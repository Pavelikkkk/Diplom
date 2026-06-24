#include "dorm_energy/infrastructure/detection/onnx_detector.hpp"

#include "dorm_energy/core/time_utils.hpp"

#include <array>
#include <cmath>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>

namespace dorm_energy::detection
{
    namespace
    {
        using FeatureArray = std::array<float, OnnxModelConfig::FeatureCount>;

        void validateConfig(
            const OnnxModelConfig &config)
        {
            if (config.anomalyThreshold <= 0.0f)
            {
                throw std::invalid_argument("anomalyThreshold must be greater than zero");
            }

            for (const auto scale : config.scale)
            {
                if (scale == 0.0f)
                {
                    throw std::invalid_argument("ONNX feature scale must not be zero");
                }
            }
        }

        FeatureArray extractFeatures(
            const DetectionContext &context)
        {
            const auto &state = context.current;
            const int hour = core::extractLocalHour(state.timestamp);

            const float angle =
                2.0f *
                std::numbers::pi_v<float> *
                static_cast<float>(hour) /
                24.0f;

            return FeatureArray{
                state.motion ? 1.0f : 0.0f,
                static_cast<float>(state.power),
                static_cast<float>(state.light),
                std::sin(angle),
                std::cos(angle)};
        }

        FeatureArray normalizeFeatures(
            FeatureArray features,
            const OnnxModelConfig &config)
        {
            for (std::size_t i = 0; i < features.size(); ++i)
            {
                features[i] = (features[i] - config.mean[i]) / config.scale[i];
            }

            return features;
        }

        float calculateMse(
            const FeatureArray &input,
            const float *output)
        {
            float mse = 0.0f;

            for (std::size_t i = 0; i < input.size(); ++i)
            {
                const float diff = input[i] - output[i];
                mse += diff * diff;
            }

            return mse / static_cast<float>(input.size());
        }

        AnomalyInfo makeNormal(
            float score)
        {
            AnomalyInfo info;
            info.score = score;
            return info;
        }

        AnomalyInfo makeMlAnomaly(
            float score)
        {
            AnomalyInfo info;
            info.isAnomaly = true;
            info.anomalyType = "ml_autoencoder_anomaly";
            info.description = "Autoencoder reconstruction error: " + std::to_string(score);
            info.severity = core::AlertSeverity::Warning;
            info.score = score;

            return info;
        }
    } // namespace

    OnnxDetector::OnnxDetector(
        const std::string &modelPath,
        OnnxModelConfig config)
        : config_(config),
          env_(ORT_LOGGING_LEVEL_WARNING, "onnx")
    {
        validateConfig(config_);

        options_.SetGraphOptimizationLevel(
            GraphOptimizationLevel::ORT_ENABLE_ALL);

#ifdef _WIN32
        const std::wstring widePath(modelPath.begin(), modelPath.end());

        session_ = std::make_unique<Ort::Session>(env_, widePath.c_str(), options_);
#else
        session_ = std::make_unique<Ort::Session>(env_, modelPath.c_str(), options_);
#endif
    }

    AnomalyInfo OnnxDetector::detect(
        const DetectionContext &context) const
    {
        const float error = calculateError(context);

        if (error > config_.anomalyThreshold)
        {
            return makeMlAnomaly(error);
        }

        return makeNormal(error);
    }

    float OnnxDetector::calculateError(
        const DetectionContext &context) const
    {
        auto inputValues = normalizeFeatures(extractFeatures(context), config_);

        std::array<int64_t, 2> inputShape{1, static_cast<int64_t>(OnnxModelConfig::FeatureCount)};

        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

        Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo,
                                                                 inputValues.data(), inputValues.size(),
                                                                 inputShape.data(), inputShape.size());

        const char *inputNames[] = {"input"};
        const char *outputNames[] = {"output"};

        auto outputs =
            session_->Run(Ort::RunOptions{nullptr},
                          inputNames,
                          &inputTensor, 1,
                          outputNames, 1);

        const float *output = outputs[0].GetTensorData<float>();

        return calculateMse(
            inputValues,
            output);
    }
} // namespace dorm_energy::detection

#include "dorm_energy/application/factories/detection_factory.hpp"

#include "dorm_energy/infrastructure/detection/hybrid_detector.hpp"
#include "dorm_energy/infrastructure/detection/onnx_detector.hpp"
#include "dorm_energy/infrastructure/detection/rule_based_detector.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    DetectionFactory::DetectionFactory(
        const AppConfig &config)
        : config_(config) {}

    std::unique_ptr<detection::IStateDetector> DetectionFactory::create() const
    {
        auto ruleDetector = std::make_unique<detection::RuleBasedDetector>(25.0);

        // TODO: move to config.
        auto mlDetector = std::make_unique<detection::OnnxDetector>("../../ml/models/anomaly_autoencoder.onnx");

        return std::make_unique<detection::HybridDetector>(
            std::move(ruleDetector),
            std::move(mlDetector));
    }
} // namespace dorm_energy::application::factories

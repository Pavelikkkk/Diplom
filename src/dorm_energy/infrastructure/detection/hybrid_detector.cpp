#include "dorm_energy/infrastructure/detection/hybrid_detector.hpp"

#include <stdexcept>
#include <utility>

namespace dorm_energy::detection
{
    HybridDetector::HybridDetector(
        std::unique_ptr<IStateDetector> ruleDetector,
        std::unique_ptr<IStateDetector> mlDetector)
        : ruleDetector_(std::move(ruleDetector)),
          mlDetector_(std::move(mlDetector))
    {
        if (!ruleDetector_)
        {
            throw std::invalid_argument("ruleDetector must not be null");
        }

        if (!mlDetector_)
        {
            throw std::invalid_argument("mlDetector must not be null");
        }
    }

    AnomalyInfo HybridDetector::detect(
        const DetectionContext &context) const
    {
        const auto ruleResult = ruleDetector_->detect(context);

        if (ruleResult.isAnomaly)
        {
            return ruleResult;
        }

        auto mlResult = mlDetector_->detect(context);

        if (mlResult.isAnomaly)
        {
            mlResult.description = "[ML] " + mlResult.description;
        }

        return mlResult;
    }
} // namespace dorm_energy::detection
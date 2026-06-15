#include "dorm_energy/infrastructure/detection/hybrid_detector.hpp"

namespace dorm_energy::detection
{

    HybridDetector::HybridDetector(std::unique_ptr<IStateDetector> ruleDetector,
                                   std::unique_ptr<IStateDetector> mlDetector)
        : ruleDetector_(std::move(ruleDetector)), mlDetector_(std::move(mlDetector))
    {
    }

    bool HybridDetector::isAnomaly(const DetectionContext &context) const
    {
        return detect(context).isAnomaly;
    }

    AnomalyInfo HybridDetector::detect(const DetectionContext &context) const
    {

        auto ruleResult = ruleDetector_->detect(context);

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
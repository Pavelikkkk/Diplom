#pragma once

#include "dorm_energy/application/config/rule_based_detector_config.hpp"
#include "dorm_energy/domain/detection/istate_detector.hpp"

namespace dorm_energy::detection
{
    class RuleBasedDetector : public IStateDetector
    {
    public:
        explicit RuleBasedDetector(
            RuleBasedDetectorConfig config);

        AnomalyInfo detect(
            const DetectionContext &context) const override;

    private:
        RuleBasedDetectorConfig config_;
    };

} // namespace dorm_energy::detection

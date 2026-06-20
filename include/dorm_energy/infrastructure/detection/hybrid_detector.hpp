#pragma once

#include "dorm_energy/domain/detection/istate_detector.hpp"

#include <memory>

namespace dorm_energy::detection
{

    class HybridDetector final : public IStateDetector
    {
    public:
        HybridDetector(
            std::unique_ptr<IStateDetector> ruleDetector,
            std::unique_ptr<IStateDetector> mlDetector);

        AnomalyInfo detect(
            const DetectionContext &context) const override;

    private:
        std::unique_ptr<IStateDetector> ruleDetector_;

        std::unique_ptr<IStateDetector> mlDetector_;
    };

} // namespace dorm_energy::detection
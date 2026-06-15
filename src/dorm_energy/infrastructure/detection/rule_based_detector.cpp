#include "dorm_energy/infrastructure/detection/rule_based_detector.hpp"
#include "dorm_energy/core/detection_context.hpp"

#include <chrono>

namespace dorm_energy::detection
{

    RuleBasedDetector::RuleBasedDetector(double maxPowerKw) : maxPowerKw_(maxPowerKw)
    {
        if (maxPowerKw_ <= 0.0)
            maxPowerKw_ = 25.0;
    }

    bool RuleBasedDetector::isAnomaly(const DetectionContext &context) const
    {
        return detect(context).isAnomaly;
    }

    AnomalyInfo RuleBasedDetector::detect(const DetectionContext &context) const
    {
        const auto &state = context.current;

        AnomalyInfo info;

        //
        // 1. Высокое потребление
        //
        if (state.power > maxPowerKw_)
        {
            info.isAnomaly = true;

            info.anomalyType = "rule_high_power";

            info.description = "Power consumption exceeds threshold";

            info.severity = core::AlertSeverity::Warning;

            return info;
        }

        //
        // 2. Свет включён без движения
        //
        if (!state.motion && state.light > 500)
        {
            info.isAnomaly = true;

            info.anomalyType = "rule_light_without_motion";

            info.description = "Light is on but no motion detected";

            info.severity = core::AlertSeverity::Info;

            return info;
        }

        //
        // 3. Нет движения 30 минут
        //    но есть высокое потребление
        //
        if (context.history && noMotionFor30Minutes(*context.history) && state.power > 5.0)
        {
            info.isAnomaly = true;

            info.anomalyType = "rule_power_without_motion_30m";

            info.description = "Power consumption without motion for 30 minutes";

            info.severity = core::AlertSeverity::Warning;

            return info;
        }

        return info;
    }

    bool RuleBasedDetector::noMotionFor30Minutes(const std::deque<core::RoomState> &history) const
    {
        if (history.size() < 2)
            return false;

        auto duration = history.back().timestamp - history.front().timestamp;

        if (duration < std::chrono::minutes(30))
        {
            return false;
        }

        for (const auto &state : history)
        {
            if (state.motion)
            {
                return false;
            }
        }

        return true;
    }

} // namespace dorm_energy::detection
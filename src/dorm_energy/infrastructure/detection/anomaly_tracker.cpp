#include "dorm_energy/infrastructure/detection/anomaly_tracker.hpp"

namespace dorm_energy::detection
{

    std::string AnomalyTracker::makeKey(
        const core::RoomState &state,
        const AnomalyInfo &anomaly) const
    {
        return state.deviceId + ":" + sensorGroup(anomaly) + ":" + anomaly.anomalyType;
    }

    std::string AnomalyTracker::sensorGroup(
        const AnomalyInfo &anomaly) const
    {
        if (anomaly.anomalyType.find("light") != std::string::npos)
        {
            return "light";
        }

        if (anomaly.anomalyType.find("motion") != std::string::npos)
        {
            return "motion";
        }

        return "power";
    }

    bool AnomalyTracker::shouldReport(
        const core::RoomState &state,
        const AnomalyInfo &anomaly)
    {
        const auto key = makeKey(state, anomaly);
        const auto now = state.timestamp;
        const auto existing = lastReportedAt_.find(key);

        if (existing != lastReportedAt_.end() && now - existing->second < Cooldown)
        {
            return false;
        }

        lastReportedAt_[key] = now;
        return true;
    }
    
} // namespace dorm_energy::detection

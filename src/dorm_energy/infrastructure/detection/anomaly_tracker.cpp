#include "dorm_energy/infrastructure/detection/anomaly_tracker.hpp"
#include "dorm_energy/core/string_utils.hpp"

namespace dorm_energy::detection
{

    std::string AnomalyTracker::makeKey(
        const core::RoomState &state,
        const AnomalyInfo &anomaly) const
    {
        return state.deviceId + ":" + anomaly.anomalyType;
    }

    bool AnomalyTracker::shouldReport(
        const core::RoomState &state,
        const AnomalyInfo &anomaly)
    {
        const auto [_, inserted] = active_.insert(makeKey(state, anomaly));
        return inserted;
    }

    void AnomalyTracker::resolveRoom(
        const std::string &deviceId)
    {
        const std::string prefix = deviceId + ":";

        for (auto it = active_.begin(); it != active_.end();)
        {
            if (core::startsWith(*it, prefix))
            {
                it = active_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
} // namespace dorm_energy::detection
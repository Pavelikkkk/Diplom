#pragma once

#include "dorm_energy/core/room_state.hpp"
#include "dorm_energy/domain/detection/anomaly_info.hpp"

#include <string>
#include <unordered_set>

namespace dorm_energy::detection
{

    class AnomalyTracker
    {
    public:
        bool shouldReport(
            const core::RoomState &state,
            const AnomalyInfo &anomaly);

        void resolveRoom(
            const std::string &deviceId);

    private:
        std::unordered_set<std::string> active_;

        std::string makeKey(
            const core::RoomState &state,
            const AnomalyInfo &anomaly) const;
    };

} // namespace dorm_energy::detection
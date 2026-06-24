#pragma once

#include "dorm_energy/core/room_state.hpp"
#include "dorm_energy/domain/detection/anomaly_info.hpp"

#include <chrono>
#include <string>
#include <unordered_map>

namespace dorm_energy::detection
{

    class AnomalyTracker
    {
    public:
        bool shouldReport(
            const core::RoomState &state,
            const AnomalyInfo &anomaly);

    private:
        static constexpr std::chrono::minutes Cooldown{10}; // убрать в кофиг

        std::unordered_map<std::string, std::chrono::system_clock::time_point> lastReportedAt_;

        std::string makeKey(
            const core::RoomState &state,
            const AnomalyInfo &anomaly) const;

        std::string sensorGroup(
            const AnomalyInfo &anomaly) const;
    };

} // namespace dorm_energy::detection

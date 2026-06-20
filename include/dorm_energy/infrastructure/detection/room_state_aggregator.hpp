#pragma once

#include "dorm_energy/application/config/room_state_aggregator_config.hpp"
#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/core/room_state.hpp"
#include "dorm_energy/domain/detection/room_state_update_result.hpp"

#include <deque>
#include <string>
#include <unordered_map>

namespace dorm_energy::detection
{
    class RoomStateAggregator
    {
    public:
        explicit RoomStateAggregator(
            RoomStateAggregatorConfig config = {});

        RoomStateUpdateResult update(
            const core::SensorReading &reading);

        void commitState(
            const core::RoomState &state);

        const std::deque<core::RoomState> &getHistory(
            const std::string &deviceId) const;

        const std::unordered_map<std::string, core::RoomState> &getCurrentStates() const;

    private:
        struct AggregatedRoomState
        {
            core::RoomState state{};

            bool hasMotion{false};

            bool hasPower{false};

            bool hasLight{false};
        };

        bool isComplete(
            const AggregatedRoomState &aggregated) const;

        void trimHistory(
            const std::string &deviceId,
            core::TimePoint currentTimestamp);

        RoomStateAggregatorConfig config_;

        std::unordered_map<std::string, AggregatedRoomState> states_;

        std::unordered_map<std::string, core::RoomState> currentStates_;

        std::unordered_map<std::string, std::deque<core::RoomState>> history_;
    };
}

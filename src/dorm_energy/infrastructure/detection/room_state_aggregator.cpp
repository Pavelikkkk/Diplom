#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"

namespace dorm_energy::detection
{

    std::optional<core::RoomState> RoomStateAggregator::update(const core::SensorReading &reading)
    {
        auto &state = states_[reading.deviceId];

        state.deviceId = reading.deviceId;

        state.timestamp = reading.timestamp;

        if (reading.sensorType == "motion")
        {
            state.motion = reading.boolValue.value_or(false);
        }
        else if (reading.sensorType == "power")
        {
            state.power = reading.value;
        }
        else if (reading.sensorType == "light")
        {
            state.light = reading.value;
        }

        auto &roomHistory = history_[reading.deviceId];

        roomHistory.push_back(state);

        while (!roomHistory.empty())
        {
            auto age = state.timestamp - roomHistory.front().timestamp;

            if (age > std::chrono::minutes(30))
            {
                roomHistory.pop_front();
            }
            else
            {
                break;
            }
        }

        return state;
    }

    const std::deque<core::RoomState> &
    RoomStateAggregator::getHistory(const std::string &deviceId) const
    {
        static const std::deque<core::RoomState> emptyHistory;

        auto it = history_.find(deviceId);

        if (it == history_.end())
        {
            return emptyHistory;
        }

        return it->second;
    }

    const std::unordered_map<std::string, core::RoomState> &
    RoomStateAggregator::getCurrentStates() const
    {
        return states_;
    }

} // namespace dorm_energy::detection
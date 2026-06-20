#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"

#include <cmath>
#include <stdexcept>

namespace dorm_energy::detection
{
    namespace
    {
        void validateConfig(
            const RoomStateAggregatorConfig &config)
        {
            if (config.historyWindow <= std::chrono::minutes{0})
            {
                throw std::invalid_argument("historyWindow must be greater than zero");
            }
        }

        bool isInvalidNumber(
            double value)
        {
            return !std::isfinite(value);
        }
    }

    RoomStateAggregator::RoomStateAggregator(
        RoomStateAggregatorConfig config)
        : config_(config)
    {
        validateConfig(config_);
    }

    RoomStateUpdateResult RoomStateAggregator::update(
        const core::SensorReading &reading)
    {
        auto &aggregated = states_[reading.deviceId];
        auto &state = aggregated.state;

        state.deviceId = reading.deviceId;
        state.timestamp = reading.timestamp;

        if (reading.sensorType == "motion")
        {
            if (!reading.boolValue.has_value())
            {
                return RoomStateUpdateResult{std::nullopt, SensorHealthStatus::InvalidMotionValue};
            }

            state.motion = reading.boolValue.value();
            aggregated.hasMotion = true;
        }
        else if (reading.sensorType == "power")
        {
            if (isInvalidNumber(reading.value))
            {
                return RoomStateUpdateResult{std::nullopt, SensorHealthStatus::InvalidNumericValue};
            }

            state.power = reading.value;
            aggregated.hasPower = true;
        }
        else if (reading.sensorType == "light")
        {
            if (isInvalidNumber(reading.value))
            {
                return RoomStateUpdateResult{std::nullopt, SensorHealthStatus::InvalidNumericValue};
            }

            state.light = reading.value;
            aggregated.hasLight = true;
        }
        else
        {
            return RoomStateUpdateResult{std::nullopt, SensorHealthStatus::UnknownSensorType};
        }

        currentStates_[reading.deviceId] = state;

        if (!isComplete(aggregated))
        {
            return RoomStateUpdateResult{std::nullopt, SensorHealthStatus::Incomplete};
        }

        return RoomStateUpdateResult{state, SensorHealthStatus::Ok};
    }

    void RoomStateAggregator::commitState(
        const core::RoomState &state)
    {
        auto &roomHistory = history_[state.deviceId];

        if (!roomHistory.empty() && roomHistory.back().timestamp == state.timestamp)
        {
            roomHistory.back() = state;
        }
        else
        {
            roomHistory.push_back(state);
        }

        trimHistory(state.deviceId, state.timestamp);
    }

    const std::deque<core::RoomState> &RoomStateAggregator::getHistory(
        const std::string &deviceId) const
    {
        static const std::deque<core::RoomState> emptyHistory;

        const auto it = history_.find(deviceId);

        if (it == history_.end())
        {
            return emptyHistory;
        }

        return it->second;
    }

    const std::unordered_map<std::string, core::RoomState> &RoomStateAggregator::getCurrentStates() const
    {
        return currentStates_;
    }

    bool RoomStateAggregator::isComplete(
        const AggregatedRoomState &aggregated) const
    {
        return aggregated.hasMotion && aggregated.hasPower && aggregated.hasLight;
    }

    void RoomStateAggregator::trimHistory(
        const std::string &deviceId,
        core::TimePoint currentTimestamp)
    {
        auto it = history_.find(deviceId);

        if (it == history_.end())
        {
            return;
        }

        auto &roomHistory = it->second;

        const auto from = currentTimestamp - config_.historyWindow;

        while (!roomHistory.empty() && roomHistory.front().timestamp < from)
        {
            roomHistory.pop_front();
        }
    }
} // namespace dorm_energy::detection

#pragma once

#include "dorm_energy/core/room_state.hpp"
#include "dorm_energy/domain/detection/sensor_health_status.hpp"

#include <optional>

namespace dorm_energy::detection
{
    struct RoomStateUpdateResult
    {
        std::optional<core::RoomState> state{};

        SensorHealthStatus health{SensorHealthStatus::Ok};

        bool has_value() const
        {
            return state.has_value();
        }

        explicit operator bool() const
        {
            return state.has_value();
        }

        core::RoomState &operator*()
        {
            return *state;
        }

        const core::RoomState &operator*() const
        {
            return *state;
        }

        core::RoomState *operator->()
        {
            return &*state;
        }

        const core::RoomState *operator->() const
        {
            return &*state;
        }
    };
}

#pragma once

#include "dorm_energy/core/room_state.hpp"

#include <deque>

namespace dorm_energy::detection
{

    struct DetectionContext
    {
        core::RoomState current{};

        const std::deque<core::RoomState> *history{nullptr};
    };

} // namespace dorm_energy::detection
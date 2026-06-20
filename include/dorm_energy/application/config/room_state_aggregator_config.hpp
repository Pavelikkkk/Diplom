#pragma once

#include <chrono>

namespace dorm_energy::detection
{
    struct RoomStateAggregatorConfig
    {
        std::chrono::minutes historyWindow{120};
    };
} // namespace dorm_energy::detection

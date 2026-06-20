#pragma once

#include "dorm_energy/core/aliases.hpp"

#include <chrono>
#include <string>

namespace dorm_energy::core
{

    struct RoomState
    {
        std::string deviceId{};

        TimePoint timestamp;

        bool motion{false};

        double power{0.0};

        double light{0.0};
    };

} // namespace dorm_energy::core

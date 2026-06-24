#pragma once

#include "dorm_energy/core/room_state.hpp"

#include <deque>
#include <optional>

namespace dorm_energy::detection
{

    struct DetectionContext
    {
        core::RoomState current{};

        const std::deque<core::RoomState> *history{nullptr};

        std::optional<double> minNormalPowerKw{};

        std::optional<double> maxNormalPowerKw{};

        bool allowUnattendedPower{false};

        std::optional<double> baselineAveragePowerKw{};

        int baselinePowerSampleCount{0};

        bool baselineReady{false};
    };

} // namespace dorm_energy::detection

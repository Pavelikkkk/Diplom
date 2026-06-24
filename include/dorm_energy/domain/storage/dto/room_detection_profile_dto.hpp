#pragma once

namespace dorm_energy::storage
{
    struct RoomDetectionProfileDto
    {
        double minNormalPowerKw{0.0};
        double maxNormalPowerKw{2.8};
        bool allowUnattendedPower{false};
        double baselineAveragePowerKw{0.0};
        int baselinePowerSampleCount{0};
        bool baselineReady{false};
    };
} // namespace dorm_energy::storage

#pragma once

#include <chrono>
#include <vector>

namespace dorm_energy::core
{
    struct SensorReading;

    using TimePoint = std::chrono::system_clock::time_point;
    using ReadingsBatch = std::vector<SensorReading>;
} // namespace dorm_energy::core

#pragma once

#include "dorm_energy/core/aliases.hpp"

#include <optional>
#include <string>

namespace dorm_energy::core
{
    struct SensorReading
    {
        TimePoint timestamp{};

        std::string deviceId{};

        std::string sensorType{};

        double value{};

        std::optional<bool> boolValue{};

        std::string unit{};
    };

} // namespace dorm_energy::core

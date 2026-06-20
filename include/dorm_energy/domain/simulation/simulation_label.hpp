#pragma once

#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/aliases.hpp"

#include <string>

namespace dorm_energy::simulation
{
    struct SimulationLabel
    {
        core::TimePoint timestamp;
        std::string deviceId;
        bool isAnomaly{false};
        std::string anomalyType;
        std::string description;
        core::AlertSeverity severity{core::AlertSeverity::Info};
    };
} // namespace dorm_energy::simulation

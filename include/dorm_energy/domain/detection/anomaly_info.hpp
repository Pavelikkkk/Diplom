#pragma once

#include "dorm_energy/core/alert_severity.hpp"

#include <string>

namespace dorm_energy::detection
{
    struct AnomalyInfo
    {
        bool isAnomaly{false};

        std::string anomalyType{};
        std::string description{};

        core::AlertSeverity severity{core::AlertSeverity::Info};

        float score{0.0f};
    };
} // namespace dorm_energy::detection
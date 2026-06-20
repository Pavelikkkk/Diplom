#pragma once

#include <string>

namespace dorm_energy::core
{
    enum class AlertSeverity
    {
        Info,
        Warning,
        Critical
    };

    std::string toString(AlertSeverity severity);
} // namespace dorm_energy::core

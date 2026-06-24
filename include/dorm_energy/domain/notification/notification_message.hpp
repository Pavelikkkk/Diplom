#pragma once

#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/aliases.hpp"

#include <string>

namespace dorm_energy::notification
{
    struct NotificationMessage
    {
        std::string title{};

        std::string body{};

        core::AlertSeverity severity{core::AlertSeverity::Info};

        std::string deviceId{};

        core::TimePoint timestamp{};

        std::string anomalyType{};

        double powerKw{0.0};

        double lightLux{0.0};

        bool motion{false};

        float score{0.0f};
    };
}

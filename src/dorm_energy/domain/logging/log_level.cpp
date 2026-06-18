#include "dorm_energy/domain/logging/log_level.hpp"

#include <algorithm>
#include <cctype>

namespace dorm_energy::logging
{
    namespace
    {
        std::string toLower(std::string value)
        {
            std::transform(
                value.begin(),
                value.end(),
                value.begin(),
                [](unsigned char c){return std::tolower(c);}
            );
            return value;
        }
    }

    LogLevel parseLogLevel(const std::string &value)
    {
        const std::string normalized = toLower(value);

        if (normalized == "debug")
            return LogLevel::Debug;
        if (normalized == "info")
            return LogLevel::Info;
        if (normalized == "warn" || normalized == "warning")
            return LogLevel::Warn;
        if (normalized == "error")
            return LogLevel::Error;
        if (normalized == "critical")
            return LogLevel::Critical;

        return LogLevel::Info;
    }

    const char *toString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Debug:
            return "debug";
        case LogLevel::Info:
            return "info";
        case LogLevel::Warn:
            return "warn";
        case LogLevel::Error:
            return "error";
        case LogLevel::Critical:
            return "critical";
        default:
            return "info";
        }
    }
}
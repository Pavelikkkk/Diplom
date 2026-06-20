#pragma once

#include <string>

namespace dorm_energy::logging
{
    enum class LogLevel
    {
        Debug,
        Info,
        Warn,
        Error,
        Critical
    };

    LogLevel parseLogLevel(const std::string &value);
}
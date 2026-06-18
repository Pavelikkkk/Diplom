// include/dorm_energy/domain/logging/ilogger.hpp
#pragma once

#include "dorm_energy/domain/logging/log_level.hpp"

#include <string>

namespace dorm_energy::logging
{
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        virtual void log(LogLevel level, const std::string &message) = 0;

        void debug(const std::string &message)
        {
            log(LogLevel::Debug, message);
        }

        void info(const std::string &message)
        {
            log(LogLevel::Info, message);
        }

        void warn(const std::string &message)
        {
            log(LogLevel::Warn, message);
        }

        void error(const std::string &message)
        {
            log(LogLevel::Error, message);
        }

        void critical(const std::string &message)
        {
            log(LogLevel::Critical, message);
        }
    };
} // namespace dorm_energy::logging
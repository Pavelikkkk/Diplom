#include "dorm_energy/infrastructure/logging/spdlog_logger.hpp"
#include "dorm_energy/application/config/app_config.hpp"
#include "dorm_energy/domain/logging/log_level.hpp"

#include <spdlog/common.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace dorm_energy::logging
{
    spdlog::level::level_enum SpdlogLogger::toSpdlogLevel(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Debug:
            return spdlog::level::debug;
        case LogLevel::Info:
            return spdlog::level::info;
        case LogLevel::Warn:
            return spdlog::level::warn;
        case LogLevel::Error:
            return spdlog::level::err;
        case LogLevel::Critical:
            return spdlog::level::critical;
        default:
            return spdlog::level::info;
        }
    }

    SpdlogLogger::SpdlogLogger(
        const application::AppConfig &config,
        const std::string &name)
        : logger_(spdlog::stdout_color_mt(name))
    {
        const LogLevel configuredLevel = parseLogLevel(config.getLogLevel());

        logger_->set_level(toSpdlogLevel(configuredLevel));
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        if (config.isVerbose())
        {
            logger_->info("SpdlogLogger initialized in verbose mode");
        }
    }

    void SpdlogLogger::log(LogLevel level, const std::string &message)
    {
        logger_->log(toSpdlogLevel(level), message);
    }
} // namespace dorm_energy::logging
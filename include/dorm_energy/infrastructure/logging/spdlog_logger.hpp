#pragma once

#include "dorm_energy/domain/logging/ilogger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

namespace dorm_energy::application
{
    class AppConfig;
}

namespace dorm_energy::logging
{
    class SpdlogLogger : public ILogger
    {
    public:
        explicit SpdlogLogger(
            const application::AppConfig &config,
            const std::string &name = "SoSM");

        void log(LogLevel level, const std::string &message) override;

    private:
        std::shared_ptr<spdlog::logger> logger_;

        static spdlog::level::level_enum toSpdlogLevel(LogLevel level);
    };
} // namespace dorm_energy::logging
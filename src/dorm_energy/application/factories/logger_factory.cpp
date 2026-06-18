#include "dorm_energy/application/factories/logger_factory.hpp"

#include "dorm_energy/infrastructure/logging/spdlog_logger.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    LoggerFactory::LoggerFactory(
        const AppConfig &config)
        : config_(config) {}

    std::shared_ptr<logging::ILogger> LoggerFactory::create()
    {
        if (!logger_)
        {
            logger_ = std::make_shared<logging::SpdlogLogger>(config_);
        }

        return logger_;
    }
} // namespace dorm_energy::application::factories

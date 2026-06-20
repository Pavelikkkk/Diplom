#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::logging
{
    class ILogger;
}

namespace dorm_energy::application::factories
{
    class LoggerFactory
    {
    public:
        explicit LoggerFactory(
            const AppConfig &config);

        std::shared_ptr<logging::ILogger> create();

    private:
        const AppConfig &config_;
        std::shared_ptr<logging::ILogger> logger_{};
    };
} // namespace dorm_energy::application::factories

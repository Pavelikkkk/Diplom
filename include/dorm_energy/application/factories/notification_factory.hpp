#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::application
{
    class INotifier;
} // namespace dorm_energy::application

namespace dorm_energy::application::factories
{
    class NotificationFactory
    {
    public:
        explicit NotificationFactory(
            const AppConfig &config);

        std::unique_ptr<INotifier> create() const;

    private:
        const AppConfig &config_;
    };
} // namespace dorm_energy::application::factories

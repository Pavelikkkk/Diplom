#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::notification
{
    class INotifier;
} // namespace dorm_energy::notification

namespace dorm_energy::application::factories
{
    class NotificationFactory
    {
    public:
        explicit NotificationFactory(
            const AppConfig &config);

        std::unique_ptr<notification::INotifier> create() const;

    private:
        const AppConfig &config_;
    };
} // namespace dorm_energy::application::factories

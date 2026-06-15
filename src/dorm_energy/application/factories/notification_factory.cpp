#include "dorm_energy/application/factories/notification_factory.hpp"

#include "dorm_energy/application/inotifier.hpp"
#include "dorm_energy/application/notifier_service.hpp"
#include "dorm_energy/infrastructure/notifier/telegram_notifier.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    NotificationFactory::NotificationFactory(
        const AppConfig &config)
        : config_(config) {}

    std::unique_ptr<INotifier> NotificationFactory::create() const
    {
        auto service = std::make_unique<NotifierService>();

        if (config_.isTelegramEnabled())
        {
            service->addNotifier(std::make_unique<notifier::TelegramNotifier>(config_));
        }

        return service;
    }
} // namespace dorm_energy::application::factories

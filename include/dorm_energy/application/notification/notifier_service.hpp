#pragma once

#include "dorm_energy/domain/notification/inotifier.hpp"

#include <memory>
#include <vector>

namespace dorm_energy::application
{
    class NotifierService final : public notification::INotifier
    {
    public:
        NotifierService() = default;

        void addNotifier(
            std::unique_ptr<notification::INotifier> notifier);

        bool send(
            const notification::NotificationMessage &message) override;

        std::size_t sendBatch(
            const std::vector<notification::NotificationMessage> &messages) override;

    private:
        std::vector<std::unique_ptr<notification::INotifier>> notifiers_;
    };
} // namespace dorm_energy::application

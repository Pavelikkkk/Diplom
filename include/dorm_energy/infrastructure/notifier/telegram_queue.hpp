#pragma once

#include "dorm_energy/infrastructure/notifier/queued_notification.hpp"

#include <mutex>
#include <vector>

namespace dorm_energy::notifier
{
    class TelegramQueue
    {
    public:
        explicit TelegramQueue(
            std::size_t maxQueueSize = 500);

        void push(
            const notification::NotificationMessage &message);

        std::vector<QueuedNotification> getAllAndClear();

        bool empty() const;

        std::size_t size() const;

    private:
        std::vector<QueuedNotification> queue_;

        mutable std::mutex mutex_;

        std::size_t maxQueueSize_;
    };
} // namespace dorm_energy::notifier

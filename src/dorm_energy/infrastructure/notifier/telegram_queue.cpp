#include "dorm_energy/infrastructure/notifier/telegram_queue.hpp"

#include <chrono>
#include <utility>

namespace dorm_energy::notifier
{
    TelegramQueue::TelegramQueue(
        std::size_t maxQueueSize)
        : maxQueueSize_(maxQueueSize)
    {
    }

    void TelegramQueue::push(
        const notification::NotificationMessage &message)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (queue_.size() >= maxQueueSize_)
        {
            queue_.erase(queue_.begin());
        }

        QueuedNotification queued;
        queued.message = message;
        queued.queuedAt = std::chrono::system_clock::now();

        queue_.push_back(std::move(queued));
    }

    std::vector<QueuedNotification> TelegramQueue::getAllAndClear()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        auto result = std::move(queue_);

        queue_.clear();

        return result;
    }

    bool TelegramQueue::empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return queue_.empty();
    }

    std::size_t TelegramQueue::size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);

        return queue_.size();
    }
} // namespace dorm_energy::notifier

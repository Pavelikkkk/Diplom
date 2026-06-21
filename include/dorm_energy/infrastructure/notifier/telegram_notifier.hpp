#pragma once

#include "dorm_energy/domain/notification/inotifier.hpp"
#include "dorm_energy/infrastructure/notifier/telegram_config.hpp"
#include "dorm_energy/infrastructure/notifier/telegram_queue.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

namespace dorm_energy::notifier
{
    class TelegramNotifier final : public notification::INotifier
    {
    public:
        explicit TelegramNotifier(
            TelegramConfig config);

        ~TelegramNotifier() override;

        bool send(
            const notification::NotificationMessage &message) override;

        std::size_t sendBatch(
            const std::vector<notification::NotificationMessage> &messages) override;

    private:
        bool sendMessage(
            const std::string &text);

        std::string buildTelegramText(
            const notification::NotificationMessage &message) const;

        void startQueueWorker();

        void stopQueueWorker();

        void queueWorker();

        bool flushQueue();

        void logQueueStatus();

    private:
        TelegramConfig config_;

        std::string apiUrl_;

        TelegramQueue queue_;

        std::thread workerThread_;

        std::atomic<bool> running_{false};

        std::atomic<std::chrono::seconds> currentBackoff_{
            std::chrono::seconds{5}};
    };
}

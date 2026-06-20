#pragma once

#include "dorm_energy/application/inotifier.hpp"
#include "dorm_energy/core/room_state.hpp"
#include "dorm_energy/domain/detection/anomaly_info.hpp"
#include "dorm_energy/infrastructure/notifier/telegram_config.hpp"
#include "dorm_energy/infrastructure/notifier/telegram_queue.hpp"
#include <atomic>
#include <chrono>
#include <thread>

namespace dorm_energy::notifier
{
    class TelegramNotifier : public application::INotifier
    {
    public:
        explicit TelegramNotifier(const application::AppConfig &config);
        explicit TelegramNotifier(TelegramConfig cfg);
        ~TelegramNotifier();

        bool sendAlert(const core::RoomState &state, const detection::AnomalyInfo &info) override;

        std::size_t sendAlerts(const std::vector<core::RoomState> &states,
                               const detection::AnomalyInfo &info) override;

    private:
        TelegramConfig config_;
        std::string apiUrl_;
        TelegramQueue queue_;

        std::thread workerThread_;
        std::atomic<bool> running_{false};
        std::atomic<std::chrono::seconds> currentBackoff_{std::chrono::seconds{5}};

        bool sendMessage(const std::string &text);
        std::string buildAlertMessage(const core::RoomState &state,
                                      const detection::AnomalyInfo &info) const;

        void startQueueWorker();
        void stopQueueWorker();
        void queueWorker();
        bool flushQueue();
        void logQueueStatus();
    };
} // namespace dorm_energy::notifier

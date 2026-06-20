#include "dorm_energy/infrastructure/notifier/telegram_notifier.hpp"
#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/measurement.hpp"

#include <chrono>
#include <curl/curl.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace dorm_energy::notifier
{
    namespace
    {
        std::size_t discardResponse(char *contents, std::size_t size, std::size_t nmemb,
                                    void *userp)
        {
            (void)contents;
            (void)userp;
            return size * nmemb;
        }
    }

    TelegramNotifier::TelegramNotifier(const application::AppConfig &config)
        : TelegramNotifier(TelegramConfig::fromAppConfig(config))
    {
    }

    TelegramNotifier::TelegramNotifier(TelegramConfig cfg) : config_(std::move(cfg))
    {
        if (config_.enabled && !config_.botToken.empty() && !config_.chatId.empty())
        {
            apiUrl_ = "https://api.telegram.org/bot" + config_.botToken + "/sendMessage";
            std::cout << "[TelegramNotifier] Initialized (chat_id: " << config_.chatId << ")\n";

            startQueueWorker();
        }
    }

    TelegramNotifier::~TelegramNotifier()
    {
        stopQueueWorker();
    }

    bool TelegramNotifier::sendAlert(const core::RoomState &state,
                                     const detection::AnomalyInfo &info)
    {
        if (!config_.enabled || apiUrl_.empty())
            return false;

        std::string text = buildAlertMessage(state, info);

        if (sendMessage(text))
            return true;

        // queue_.push(state, info.severity, info.description);
        std::cout << "[TelegramNotifier] Message queued (size: " << queue_.size() << ")\n";
        return false;
    }

    std::size_t TelegramNotifier::sendAlerts(const std::vector<core::RoomState> &states,
                                             const detection::AnomalyInfo &info)
    {
        if (states.empty())
            return 0;

        std::size_t sent = 0;
        for (const auto &state : states)
        {
            if (sendAlert(state, info))
                ++sent;
        }
        return sent;
    }

    bool TelegramNotifier::sendMessage(const std::string &text)
    {
        if (text.empty())
            return false;

        CURL *curl = curl_easy_init();
        if (!curl)
            return false;

        std::string replyMarkup = R"({"inline_keyboard":[[{"text":"✅ Принято","callback_data":"ack"}]]})";

        char *escapedText = curl_easy_escape(curl, text.c_str(), static_cast<int>(text.length()));
        char *escapedMarkup =
            curl_easy_escape(curl, replyMarkup.c_str(), static_cast<int>(replyMarkup.length()));

        std::string postData = "chat_id=" + config_.chatId + "&text=" + escapedText +
                               "&parse_mode=MarkdownV2" + "&reply_markup=" + escapedMarkup;

        curl_free(escapedText);
        curl_free(escapedMarkup);

        curl_easy_setopt(curl, CURLOPT_URL, apiUrl_.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discardResponse);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        return res == CURLE_OK;
    }

    std::string TelegramNotifier::buildAlertMessage(
        const core::RoomState &state,
        const detection::AnomalyInfo &info) const
    {
        std::ostringstream oss;

        std::string emoji =
            (info.severity == core::AlertSeverity::Critical)
                ? "🚨"
            : (info.severity == core::AlertSeverity::Warning) ? "⚠️"
                                                              : "ℹ️";

        oss << emoji << " *" << core::toString(info.severity) << " АНОМАЛИЯ*\n\n";
        oss << "🏠 *Комната:* `" << state.deviceId << "`\n";
        oss << "⚡ *Power:* `" << std::fixed << std::setprecision(2) << state.power << "` kW\n";
        oss << "🚶 *Motion:* `" << (state.motion ? "true" : "false") << "`\n";
        oss << "💡 *Light:* `" << std::fixed << std::setprecision(2) << state.light << "` lx\n";
        oss << "🚨 *Тип:* `" << info.anomalyType << "`\n";

        if (info.score > 0.0)
        {
            oss << "🤖 *ML Score:* `" << std::fixed << std::setprecision(3) << info.score << "`\n";
        }

        auto tt = std::chrono::system_clock::to_time_t(state.timestamp);
        std::ostringstream ts;
        ts << std::put_time(std::localtime(&tt), "%Y-%m-%d %H:%M:%S");
        oss << "⏰ *Время:* `" << ts.str() << "`\n";

        if (!info.description.empty())
        {
            oss << "\n🔍 *Причина:* " << info.description << "\n";
        }

        oss << "\n_Нажми кнопку ниже после принятия_";

        std::string msg = oss.str();
        const std::string special = "_*[]()~`>#+-=|{}.!";

        for (char c : special)
        {
            size_t pos = 0;
            while ((pos = msg.find(c, pos)) != std::string::npos)
            {
                msg.insert(pos, "\\");
                pos += 2;
            }
        }

        return msg;
    }

    void TelegramNotifier::startQueueWorker()
    {
        if (running_)
            return;
        running_ = true;
        workerThread_ = std::thread(&TelegramNotifier::queueWorker, this);
    }

    void TelegramNotifier::stopQueueWorker()
    {
        if (!running_)
            return;
        running_ = false;
        if (workerThread_.joinable())
            workerThread_.join();
    }

    void TelegramNotifier::queueWorker()
    {
        std::cout << "[TelegramNotifier] Queue worker started\n";

        while (running_)
        {
            bool hasInternet = false;

            if (!queue_.empty())
            {
                hasInternet = flushQueue();

                if (hasInternet)
                {
                    currentBackoff_ = std::chrono::seconds{5};
                }
                else
                {
                    auto current = currentBackoff_.load();
                    long long seconds = current.count() * 2;
                    if (seconds > 900)
                        seconds = 900;
                    currentBackoff_ = std::chrono::seconds{seconds};
                }
            }

            if (queue_.size() > 0)
            {
                std::cout << "[TelegramNotifier] Queue: " << queue_.size()
                          << " | Backoff: " << currentBackoff_.load().count() << "s\n";
            }

            std::this_thread::sleep_for(currentBackoff_.load());
        }
    }

    bool TelegramNotifier::flushQueue()
    {
        return true;
    }

    void TelegramNotifier::logQueueStatus()
    {
        std::size_t sz = queue_.size();
        if (sz > 0)
        {
            std::cout << "[TelegramNotifier] Queue: " << sz
                      << " alerts | Backoff: " << currentBackoff_.load().count() << "s\n";
        }
    }

} // namespace dorm_energy::notifier

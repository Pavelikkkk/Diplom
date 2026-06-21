#include "dorm_energy/infrastructure/notifier/telegram_notifier.hpp"

#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/time_utils.hpp"

#include <chrono>
#include <curl/curl.h>
#include <sstream>
#include <thread>
#include <utility>

namespace dorm_energy::notifier
{
    namespace
    {
        std::size_t discardResponse(
            char *contents,
            std::size_t size,
            std::size_t nmemb,
            void *userp)
        {
            (void)contents;
            (void)userp;

            return size * nmemb;
        }

        std::string severityEmoji(
            core::AlertSeverity severity)
        {
            switch (severity)
            {
            case core::AlertSeverity::Critical:
                return "🚨";

            case core::AlertSeverity::Warning:
                return "⚠️";

            case core::AlertSeverity::Info:
            default:
                return "ℹ️";
            }
        }
    }

    TelegramNotifier::TelegramNotifier(
        TelegramConfig config)
        : config_(std::move(config)),
          queue_(config_.maxQueueSize)
    {
        if (!config_.enabled)
        {
            return;
        }

        if (config_.botToken.empty() || config_.chatId.empty())
        {
            return;
        }

        apiUrl_ = "https://api.telegram.org/bot" + config_.botToken + "/sendMessage";

        startQueueWorker();
    }

    TelegramNotifier::~TelegramNotifier()
    {
        stopQueueWorker();
    }

    bool TelegramNotifier::send(
        const notification::NotificationMessage &message)
    {
        if (!config_.enabled || apiUrl_.empty())
        {
            return false;
        }

        const std::string text = buildTelegramText(message);

        if (sendMessage(text))
        {
            return true;
        }

        queue_.push(message);

        return false;
    }

    std::size_t TelegramNotifier::sendBatch(
        const std::vector<notification::NotificationMessage> &messages)
    {
        if (messages.empty())
        {
            return 0;
        }

        std::size_t sentCount = 0;

        for (const auto &message : messages)
        {
            if (send(message))
            {
                ++sentCount;
            }
        }

        return sentCount;
    }

    bool TelegramNotifier::sendMessage(
        const std::string &text)
    {
        if (text.empty())
        {
            return false;
        }

        CURL *curl = curl_easy_init();

        if (!curl)
        {
            return false;
        }

        char *escapedChatId = curl_easy_escape(
            curl,
            config_.chatId.c_str(),
            static_cast<int>(config_.chatId.length()));

        char *escapedText = curl_easy_escape(
            curl,
            text.c_str(),
            static_cast<int>(text.length()));

        if (!escapedChatId || !escapedText)
        {
            if (escapedChatId)
            {
                curl_free(escapedChatId);
            }

            if (escapedText)
            {
                curl_free(escapedText);
            }

            curl_easy_cleanup(curl);

            return false;
        }

        const std::string postData = "chat_id=" + std::string(escapedChatId) + "&text=" + std::string(escapedText);

        curl_free(escapedChatId);
        curl_free(escapedText);

        curl_easy_setopt(curl, CURLOPT_URL, apiUrl_.c_str());

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                         postData.c_str());

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discardResponse);

        const CURLcode result = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        return result == CURLE_OK;
    }

    std::string TelegramNotifier::buildTelegramText(
        const notification::NotificationMessage &message) const
    {
        std::ostringstream stream;

        stream << severityEmoji(message.severity) << " Dorm Energy Alert\n";

        if (!message.title.empty())
        {
            stream << "\n"
                   << message.title << "\n";
        }

        if (!message.body.empty())
        {
            stream << "\n"
                   << message.body << "\n";
        }

        if (!message.deviceId.empty())
        {
            stream << "\nDevice: " << message.deviceId;
        }

        stream
            << "\nSeverity: "
            << core::toString(message.severity);

        if (message.timestamp != core::TimePoint{})
        {
            stream << "\nTime: " << core::formatLocalTimestamp(message.timestamp);
        }

        return stream.str();
    }

    void TelegramNotifier::startQueueWorker()
    {
        if (running_)
        {
            return;
        }

        running_ = true;

        workerThread_ = std::thread(&TelegramNotifier::queueWorker, this);
    }

    void TelegramNotifier::stopQueueWorker()
    {
        if (!running_)
        {
            return;
        }

        running_ = false;

        if (workerThread_.joinable())
        {
            workerThread_.join();
        }
    }

    void TelegramNotifier::queueWorker()
    {
        while (running_)
        {
            if (!queue_.empty())
            {
                const bool flushed = flushQueue();

                if (flushed)
                {
                    currentBackoff_ = std::chrono::seconds{5};
                }
                else
                {
                    const auto current = currentBackoff_.load();

                    long long seconds = current.count() * 2;

                    if (seconds > 900)
                    {
                        seconds = 900;
                    }

                    currentBackoff_ = std::chrono::seconds{seconds};
                }
            }

            std::this_thread::sleep_for(currentBackoff_.load());
        }
    }

    bool TelegramNotifier::flushQueue()
    {
        const auto queuedMessages = queue_.getAllAndClear();

        if (queuedMessages.empty())
        {
            return true;
        }

        bool allSucceeded = true;

        for (const auto &queued : queuedMessages)
        {
            const std::string text = buildTelegramText(queued.message);

            if (!sendMessage(text))
            {
                queue_.push(queued.message);
                allSucceeded = false;
            }
        }

        return allSucceeded;
    }

    void TelegramNotifier::logQueueStatus()
    {
    }
} // namespace dorm_energy::notifier

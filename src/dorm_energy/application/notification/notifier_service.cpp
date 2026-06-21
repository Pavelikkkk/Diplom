#include "dorm_energy/application/notification/notifier_service.hpp"

namespace dorm_energy::application
{

    void NotifierService::addNotifier(std::unique_ptr<notification::INotifier> notifier)
    {
        if (notifier)
            notifiers_.push_back(std::move(notifier));
    }

    bool NotifierService::send(
        const notification::NotificationMessage &message)
    {
        if (notifiers_.empty())
            return false;

        bool success = true;
        for (const auto &notifier : notifiers_)
        {
            if (!notifier->send(message))
                success = false;
        }

        return success;
    }

    std::size_t NotifierService::sendBatch(
        const std::vector<notification::NotificationMessage> &messages)
    {
        if (notifiers_.empty() || messages.empty())
            return 0;

        std::size_t totalSent = 0;
        for (const auto &notifier : notifiers_)
        {
            totalSent += notifier->sendBatch(messages);
        }

        return totalSent;
    }

} // namespace dorm_energy::application

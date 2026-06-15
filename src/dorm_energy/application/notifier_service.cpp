#include "dorm_energy/application/notifier_service.hpp"
#include <fmt/format.h>

namespace dorm_energy::application
{

    void NotifierService::addNotifier(std::unique_ptr<INotifier> notifier)
    {
        if (notifier)
            notifiers_.push_back(std::move(notifier));
    }

    bool NotifierService::sendAlert(const core::RoomState &state,
                                    const detection::AnomalyInfo &info)
    {
        if (notifiers_.empty())
            return false;

        bool success = true;
        for (const auto &notifier : notifiers_)
        {
            if (!notifier->sendAlert(state, info))
                success = false;
        }

        return success;
    }

    std::size_t NotifierService::sendAlerts(const std::vector<core::RoomState> &states,
                                            const detection::AnomalyInfo &info)
    {
        if (notifiers_.empty() || states.empty())
            return 0;

        std::size_t totalSent = 0;
        for (const auto &notifier : notifiers_)
        {
            totalSent += notifier->sendAlerts(states, info);
        }

        return totalSent;
    }

} // namespace dorm_energy::application
#pragma once

#include "dorm_energy/domain/notification/notification_message.hpp"

#include <cstddef>
#include <vector>

namespace dorm_energy::notification
{
    class INotifier
    {
    public:
        virtual ~INotifier() = default;

        virtual bool send(
            const NotificationMessage &message) = 0;

        virtual std::size_t sendBatch(
            const std::vector<NotificationMessage> &messages) = 0;
    };
}
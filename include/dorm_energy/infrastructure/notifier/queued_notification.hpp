#pragma once

#include "dorm_energy/domain/notification/notification_message.hpp"

#include <chrono>

namespace dorm_energy::notifier
{
    struct QueuedNotification
    {
        notification::NotificationMessage message{};
        std::chrono::system_clock::time_point queuedAt{};
    };
} // namespace dorm_energy::notifier

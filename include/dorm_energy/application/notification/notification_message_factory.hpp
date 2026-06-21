#pragma once

#include "dorm_energy/core/room_state.hpp"
#include "dorm_energy/domain/detection/anomaly_info.hpp"
#include "dorm_energy/domain/notification/notification_message.hpp"

namespace dorm_energy::application
{
    notification::NotificationMessage makeAnomalyNotification(
        const core::RoomState &state,
        const detection::AnomalyInfo &info);
}

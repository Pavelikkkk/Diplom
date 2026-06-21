#include "dorm_energy/application/notification/notification_message_factory.hpp"

namespace dorm_energy::application
{
    notification::NotificationMessage makeAnomalyNotification(
        const core::RoomState &state,
        const detection::AnomalyInfo &info)
    {
        return notification::NotificationMessage{
            .title = "Anomaly detected",
            .body = info.description,
            .severity = info.severity,
            .deviceId = state.deviceId,
            .timestamp = state.timestamp,
        };
    }
}

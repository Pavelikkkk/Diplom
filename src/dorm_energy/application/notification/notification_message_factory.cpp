#include "dorm_energy/application/notification/notification_message_factory.hpp"

#include <string>

namespace dorm_energy::application
{
    namespace
    {
        std::string humanReadableAnomalyType(
            const std::string &type)
        {
            if (type == "rule_baseline_power_spike")
            {
                return "Power spike above room baseline";
            }

            if (type == "rule_repeated_power_spikes")
            {
                return "Repeated power spikes";
            }

            if (type == "rule_sudden_power_spike")
            {
                return "Sudden power spike";
            }

            if (type == "rule_sustained_high_power")
            {
                return "Sustained high power";
            }

            if (type == "rule_unattended_power_usage")
            {
                return "Power usage without motion";
            }

            if (type == "rule_extreme_power")
            {
                return "Extreme power value";
            }

            if (type == "rule_extreme_light")
            {
                return "Extreme light value";
            }

            if (type == "ml_autoencoder_anomaly")
            {
                return "Unusual room pattern";
            }

            return type.empty() ? "Anomaly detected" : type;
        }
    }

    notification::NotificationMessage makeAnomalyNotification(
        const core::RoomState &state,
        const detection::AnomalyInfo &info)
    {
        return notification::NotificationMessage{
            .title = humanReadableAnomalyType(info.anomalyType),
            .body = info.description,
            .severity = info.severity,
            .deviceId = state.deviceId,
            .timestamp = state.timestamp,
            .anomalyType = info.anomalyType,
            .powerKw = state.power,
            .lightLux = state.light,
            .motion = state.motion,
            .score = info.score,
        };
    }
}

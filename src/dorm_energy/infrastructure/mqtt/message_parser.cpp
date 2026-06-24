#include "dorm_energy/infrastructure/mqtt/message_parser.hpp"

#include <chrono>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace dorm_energy::mqtt
{
    namespace
    {
        constexpr std::size_t MaxPayloadSize = 4096; // в конфиг

        std::optional<std::string> readStringField(
            const nlohmann::json &json,
            const char *fieldName)
        {
            if (json.contains(fieldName) && json[fieldName].is_string())
            {
                return json[fieldName].get<std::string>();
            }

            return std::nullopt;
        }

        std::optional<std::string> readStringField(
            const nlohmann::json &json,
            const char *primaryFieldName,
            const char *fallbackFieldName)
        {
            auto value = readStringField(json, primaryFieldName);

            if (value.has_value())
            {
                return value;
            }

            return readStringField(json, fallbackFieldName);
        }

        std::optional<double> readDoubleField(
            const nlohmann::json &json,
            const char *fieldName)
        {
            if (json.contains(fieldName) && json[fieldName].is_number())
            {
                return json[fieldName].get<double>();
            }

            return std::nullopt;
        }

        std::optional<double> readDoubleField(
            const nlohmann::json &json,
            const char *primaryFieldName,
            const char *fallbackFieldName)
        {
            auto value = readDoubleField(json, primaryFieldName);

            if (value.has_value())
            {
                return value;
            }

            return readDoubleField(json, fallbackFieldName);
        }

        std::optional<bool> readBoolField(
            const nlohmann::json &json,
            const char *fieldName)
        {
            if (json.contains(fieldName) && json[fieldName].is_boolean())
            {
                return json[fieldName].get<bool>();
            }

            return std::nullopt;
        }
    }

    std::optional<core::SensorReading> MessageParser::parse(
        const std::string &payload) const
    {
        if (!canParse(payload))
        {
            return std::nullopt;
        }
        
        try
        {
            const auto json = nlohmann::json::parse(payload);

            const auto deviceId = readStringField(json, "deviceId", "device_id").value_or("unknown");
            const auto sensorType = readStringField(json, "sensorType", "sensor_type").value_or("unknown");
            const auto numericValue = readDoubleField(json, "value", "numeric_value");
            const auto boolValue = readBoolField(json, "boolValue").value_or(readBoolField(json, "bool_value").value_or(false));

            if (!numericValue.has_value() && sensorType != "motion")
            {
                return std::nullopt;
            }

            core::SensorReading reading;
            reading.timestamp = std::chrono::system_clock::now();
            reading.deviceId = deviceId;
            reading.sensorType = sensorType;
            reading.value = numericValue.value_or(boolValue ? 1.0 : 0.0);

            const auto unit = readStringField(json, "unit");

            if (unit.has_value())
            {
                reading.unit = unit.value();
            }

            if (reading.sensorType == "motion")
            {
                reading.boolValue = json.contains("boolValue") || json.contains("bool_value")
                    ? boolValue
                    : reading.value != 0.0;
                reading.value = reading.boolValue.value() ? 1.0 : 0.0;
                reading.unit = "bool";
            }
            else if (reading.sensorType == "light")
            {
                if (reading.unit.empty())
                {
                    reading.unit = "lux";
                }
            }
            else if (reading.sensorType == "power")
            {
                if (reading.unit == "W")
                {
                    reading.value /= 1000.0;
                    reading.unit = "kW";
                }

                if (reading.unit.empty())
                {
                    reading.unit = "kW";
                }
            }
            else
            {
                if (reading.sensorType == "unknown")
                {
                    return reading;
                }

                return std::nullopt;
            }

            return reading;
        }
        catch (const nlohmann::json::exception &)
        {
            return std::nullopt;
        }
    }

    bool MessageParser::canParse(
        const std::string &payload) const
    {
        if (payload.empty())
        {
            return false;
        }

        if (payload.size() > MaxPayloadSize)
        {
            return false;
        }

        return payload.find('{') != std::string::npos && payload.find('}') != std::string::npos;
    }
} // namespace dorm_energy::mqtt

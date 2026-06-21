#include "dorm_energy/infrastructure/mqtt/message_parser.hpp"

#include <chrono>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace dorm_energy::mqtt
{
    namespace
    {
        constexpr std::size_t MaxPayloadSize = 4096;

        std::optional<std::string> readStringField(
            const nlohmann::json &json,
            const char *camelCaseName,
            const char *snakeCaseName)
        {
            if (json.contains(camelCaseName) &&
                json[camelCaseName].is_string())
            {
                return json[camelCaseName].get<std::string>();
            }

            if (json.contains(snakeCaseName) &&
                json[snakeCaseName].is_string())
            {
                return json[snakeCaseName].get<std::string>();
            }

            return std::nullopt;
        }

        std::optional<double> readDoubleField(
            const nlohmann::json &json,
            const char *camelCaseName,
            const char *snakeCaseName)
        {
            if (json.contains(camelCaseName) &&
                json[camelCaseName].is_number())
            {
                return json[camelCaseName].get<double>();
            }

            if (json.contains(snakeCaseName) &&
                json[snakeCaseName].is_number())
            {
                return json[snakeCaseName].get<double>();
            }

            return std::nullopt;
        }

        std::optional<bool> readBoolField(
            const nlohmann::json &json,
            const char *camelCaseName,
            const char *snakeCaseName)
        {
            if (json.contains(camelCaseName) &&
                json[camelCaseName].is_boolean())
            {
                return json[camelCaseName].get<bool>();
            }

            if (json.contains(snakeCaseName) &&
                json[snakeCaseName].is_boolean())
            {
                return json[snakeCaseName].get<bool>();
            }

            return std::nullopt;
        }
    }

    std::optional<core::SensorReading>
    MessageParser::parse(
        const std::string &payload) const
    {
        if (!canParse(payload))
        {
            return std::nullopt;
        }

        try
        {
            const auto json =
                nlohmann::json::parse(payload);

            const auto deviceId =
                readStringField(
                    json,
                    "deviceId",
                    "device_id");

            const auto sensorType =
                readStringField(
                    json,
                    "sensorType",
                    "sensor_type");

            core::SensorReading reading;
            reading.timestamp =
                std::chrono::system_clock::now();
            reading.deviceId = deviceId.value_or("unknown");
            reading.sensorType = sensorType.value_or("unknown");

            const auto numericValue =
                readDoubleField(
                    json,
                    "value",
                    "numeric_value");

            if (numericValue.has_value())
            {
                reading.value =
                    numericValue.value();
            }

            const auto boolValue =
                readBoolField(
                    json,
                    "boolValue",
                    "bool_value");

            if (boolValue.has_value())
            {
                reading.boolValue =
                    boolValue.value();
            }

            const auto unit =
                readStringField(
                    json,
                    "unit",
                    "unit");

            if (unit.has_value())
            {
                reading.unit =
                    unit.value();
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

        return payload.find('{') != std::string::npos &&
               payload.find('}') != std::string::npos;
    }
} // namespace dorm_energy::mqtt

#include "dorm_energy/infrastructure/mqtt/message_parser.hpp"
#include "dorm_energy/core/measurement.hpp"

#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <chrono>
#include <iostream>

namespace dorm_energy::mqtt
{

    std::optional<core::SensorReading> MessageParser::parse(const std::string &payload) const
    {
        if (payload.empty())
        {
            std::cerr << "[MessageParser] Empty payload\n"; //rutime erro
            return std::nullopt;
        }

        try
        {
            auto j = nlohmann::json::parse(payload);

            core::SensorReading reading;
            reading.timestamp = std::chrono::system_clock::now();

            reading.deviceId = j.value("deviceId", j.value("device_id", std::string("unknown")));
            reading.sensorType = j.value("sensorType", j.value("sensor_type", std::string("unknown")));

            if (j.contains("value") || j.contains("numeric_value"))
            {
                reading.value = j.value("value", j.value("numeric_value", 0.0));
            }

            if (j.contains("boolValue") || j.contains("bool_value"))
            {
                reading.boolValue = j.value("boolValue", j.value("bool_value", false));
            }

            reading.unit = j.value("unit", std::string(""));

            std::cout << fmt::format("[MessageParser] Parsed: {} | {} | {:.2f} {}\n",
                                     reading.deviceId, reading.sensorType, reading.value, reading.unit);

            return reading;
        }
        catch (const nlohmann::json::parse_error &e)
        {
            std::cerr << "[MessageParser] JSON parse error: " << e.what() << "\nPayload: " << payload << std::endl;
            return std::nullopt;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[MessageParser] Error parsing message: " << e.what() << std::endl;
            return std::nullopt;
        }
    }

    bool MessageParser::canParse(const std::string &payload) const
    {
        if (payload.empty() || payload.size() > 4096)
            return false;
        return payload.find('{') != std::string::npos;
    }

} // namespace dorm_energy::mqtt
#pragma once

#include "dorm_energy/core/measurement.hpp"

#include <optional>
#include <string>

namespace dorm_energy::mqtt
{
    class MessageParser
    {
    public:
        MessageParser() = default;

        std::optional<core::SensorReading> parse(
            const std::string &payload) const;

        bool canParse(
            const std::string &payload) const;
    };
}

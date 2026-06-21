#pragma once

#include "dorm_energy/core/measurement.hpp"

#include <cstddef>
#include <vector>

namespace dorm_energy::application
{
    class IMessageHandler
    {
    public:
        virtual ~IMessageHandler() = default;

        virtual bool handle(
            const core::SensorReading &reading) = 0;

        virtual void flush() = 0;

        virtual std::size_t handleBatch(
            const std::vector<core::SensorReading> &readings) = 0;
    };
}
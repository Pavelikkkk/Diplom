#pragma once

#include "dorm_energy/core/measurement.hpp"

namespace dorm_energy::storage
{
    class IMeasurementRepository
    {
    public:
        virtual ~IMeasurementRepository() = default;

        virtual bool saveReading(
            const core::SensorReading &reading) = 0;

        virtual std::size_t saveBatch(
            const core::ReadingsBatch &readings) = 0;

        virtual void flush() = 0;
    };
} // namespace dorm_energy::storage

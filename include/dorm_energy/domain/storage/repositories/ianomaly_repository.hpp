#pragma once

#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/storage/dto/anomaly_dto.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace dorm_energy::storage
{
    class IAnomalyRepository
    {
    public:
        virtual ~IAnomalyRepository() = default;

        virtual bool saveAnomaly(
            const core::SensorReading &reading,
            const std::string &anomalyType,
            core::AlertSeverity severity,
            const std::string &description,
            double score = 0.0) = 0;

        virtual std::vector<AnomalyDto> getLatestAnomalies(
            std::size_t limit = 20,
            int organizationId = 0) = 0;
    };
} // namespace dorm_energy::storage

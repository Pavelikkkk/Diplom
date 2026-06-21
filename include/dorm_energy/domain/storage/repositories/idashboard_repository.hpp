#pragma once

#include "dorm_energy/domain/storage/dto/anomaly_stats_dto.hpp"
#include "dorm_energy/domain/storage/dto/energy_by_room_dto.hpp"
#include "dorm_energy/domain/storage/dto/power_point_dto.hpp"
#include "dorm_energy/domain/storage/dto/severity_stats_dto.hpp"
#include "dorm_energy/domain/storage/dto/top_consumer_dto.hpp"

#include <vector>

namespace dorm_energy::storage
{
    class IDashboardRepository
    {
    public:
        virtual ~IDashboardRepository() = default;

        virtual std::vector<PowerPointDto> getPowerHistory(
            int hours = 24,
            int organizationId = 0) = 0;

        virtual std::vector<TopConsumerDto> getTopConsumers(
            int limit = 10,
            int organizationId = 0) = 0;

        virtual std::vector<AnomalyStatsDto> getAnomalyStatistics(
            int organizationId = 0) = 0;

        virtual std::vector<EnergyByRoomDto> getEnergyByRoom(
            int organizationId = 0) = 0;

        virtual std::vector<SeverityStatsDto> getSeverityDistribution(
            int organizationId = 0) = 0;
    };
} // namespace dorm_energy::storage

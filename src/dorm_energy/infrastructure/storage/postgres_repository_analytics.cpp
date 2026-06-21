#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"

namespace dorm_energy::storage
{

    std::vector<storage::PowerPointDto> PostgresMeasurementRepository::getPowerHistory(
        int hours,
        int organizationId)
    {
        std::vector<PowerPointDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                time_bucket('1 hour', recorded_at) AS bucket,
                AVG(numeric_value) AS avg_power
            FROM sensor_readings
            JOIN devices d
                ON d.device_id = sensor_readings.device_id
            JOIN rooms r
                ON r.id = d.room_id
            JOIN buildings b
                ON b.id = r.building_id
            WHERE sensor_type = 'power'
              AND recorded_at >= NOW() - ($1 || ' hours')::interval
              AND ($2 = 0 OR b.organization_id = $2)
            GROUP BY bucket
            ORDER BY bucket
            )",
            hours, organizationId);

        for (auto const &row : rows)
        {
            PowerPointDto dto;

            dto.time = row["bucket"].c_str();
            dto.power = row["avg_power"].as<double>(0.0);

            result.push_back(dto);
        }

        return result;
    }

    std::vector<storage::TopConsumerDto> PostgresMeasurementRepository::getTopConsumers(
        int limit,
        int organizationId)
    {
        std::vector<TopConsumerDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                sensor_readings.device_id,
                AVG(numeric_value) AS avg_power
            FROM sensor_readings
            JOIN devices d
                ON d.device_id = sensor_readings.device_id
            JOIN rooms r
                ON r.id = d.room_id
            JOIN buildings b
                ON b.id = r.building_id
            WHERE sensor_type = 'power'
              AND ($2 = 0 OR b.organization_id = $2)
            GROUP BY sensor_readings.device_id
            ORDER BY avg_power DESC
            LIMIT $1
            )",
            limit, organizationId);

        for (const auto &row : rows)
        {
            TopConsumerDto dto;

            dto.roomName = row["device_id"].c_str();
            dto.power = row["avg_power"].as<double>(0.0);

            result.push_back(dto);
        }

        return result;
    }

    std::vector<storage::AnomalyStatsDto> PostgresMeasurementRepository::getAnomalyStatistics(
        int organizationId)
    {
        std::vector<AnomalyStatsDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                anomaly_type,
                COUNT(*) AS total
            FROM anomalies
            JOIN devices d
                ON d.device_id = anomalies.device_id
            JOIN rooms r
                ON r.id = d.room_id
            JOIN buildings b
                ON b.id = r.building_id
            WHERE ($1 = 0 OR b.organization_id = $1)
            GROUP BY anomaly_type
            ORDER BY total DESC
            )",
            organizationId);

        for (const auto &row : rows)
        {
            AnomalyStatsDto dto;

            dto.type = row["anomaly_type"].c_str();
            dto.count = row["total"].as<int>();

            result.push_back(dto);
        }

        return result;
    }

    std::vector<storage::EnergyByRoomDto> PostgresMeasurementRepository::getEnergyByRoom(
        int organizationId)
    {
        std::vector<EnergyByRoomDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                r.room_name,
                AVG(numeric_value) AS avg_power
            FROM sensor_readings
            JOIN devices d
                ON d.device_id = sensor_readings.device_id
            JOIN rooms r
                ON r.id = d.room_id
            JOIN buildings b
                ON b.id = r.building_id
            WHERE sensor_type = 'power'
              AND ($1 = 0 OR b.organization_id = $1)
            GROUP BY r.room_name
            ORDER BY avg_power DESC
            )",
            organizationId);

        for (auto const &row : rows)
        {
            EnergyByRoomDto dto;

            dto.roomName = row["room_name"].c_str();
            dto.avgPower = row["avg_power"].as<double>(0.0);

            result.push_back(dto);
        }

        return result;
    }

    std::vector<storage::SeverityStatsDto> PostgresMeasurementRepository::getSeverityDistribution(
        int organizationId)
    {
        std::vector<SeverityStatsDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                severity,
                COUNT(*) AS total
            FROM anomalies
            JOIN devices d
                ON d.device_id = anomalies.device_id
            JOIN rooms r
                ON r.id = d.room_id
            JOIN buildings b
                ON b.id = r.building_id
            WHERE ($1 = 0 OR b.organization_id = $1)
            GROUP BY severity
            ORDER BY total DESC
            )",
            organizationId);

        for (auto const &row : rows)
        {
            SeverityStatsDto dto;

            dto.severity = row["severity"].c_str();
            dto.count = row["total"].as<int>(0);

            result.push_back(dto);
        }

        return result;
    }

} // namespace dorm_energy::storage

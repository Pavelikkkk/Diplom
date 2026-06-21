#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"
#include "dorm_energy/core/alert_severity.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <pqxx/params>
#include <spdlog/spdlog.h>

namespace dorm_energy::storage
{
    bool PostgresMeasurementRepository::saveAnomaly(
        const core::SensorReading &reading,
        const std::string &anomalyType,
        core::AlertSeverity severity,
        const std::string &description, double score)
    {
        try
        {
            if (!connection_ || !connection_->is_open())
            {
                if (!tryReconnect(3))
                    return false;
            }

            pqxx::work txn{*connection_};

            std::string ts = fmt::format("{:%Y-%m-%d %H:%M:%S%z}", reading.timestamp);
            std::optional<bool> boolVal = reading.boolValue;

            ensureDeviceExists(txn, reading.deviceId);

            txn.exec(
                R"(INSERT INTO anomalies
            (recorded_at, device_id, sensor_type, numeric_value, bool_value, unit, anomaly_type, severity, description, score)
           VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10))",
                pqxx::params{ts, reading.deviceId, reading.sensorType, reading.value, boolVal, reading.unit, anomalyType, core::toString(severity), description, score});

            txn.commit();

            spdlog::info("[Postgres] Anomaly saved: {} - {} ({})", anomalyType, reading.deviceId,
                         core::toString(severity));

            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("[Postgres] Save anomaly error: {}", e.what());
            return false;
        }
    }

    std::vector<storage::AnomalyDto> PostgresMeasurementRepository::getLatestAnomalies(
        std::size_t limit,
        int organizationId)
    {
        std::vector<AnomalyDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                anomalies.device_id,
                anomalies.anomaly_type,
                anomalies.severity,
                anomalies.score,
                anomalies.description,
                anomalies.detected_at,
                r.room_name
            FROM anomalies
            JOIN devices d
                ON d.device_id = anomalies.device_id
            JOIN rooms r
                ON r.id = d.room_id
            JOIN buildings b
                ON b.id = r.building_id
            WHERE ($2 = 0 OR b.organization_id = $2)
            ORDER BY detected_at DESC
            LIMIT $1
            )",
            limit, organizationId);

        for (auto const &row : rows)
        {
            AnomalyDto dto;

            dto.room = row["room_name"].c_str();
            dto.type = row["anomaly_type"].c_str();
            dto.severity = row["severity"].c_str();
            dto.score = row["score"].as<double>(0.0);
            dto.description = row["description"].c_str();
            dto.detectedAt = row["detected_at"].c_str();

            result.push_back(dto);
        }

        return result;
    }
} // namespace dorm_energy::storage

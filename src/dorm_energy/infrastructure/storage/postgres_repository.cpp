#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"
#include "dorm_energy/core/alert_severity.hpp"

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <iostream>
#include <pqxx/params>
#include <pqxx/stream_to>
#include <thread>
#include <unordered_set>

namespace dorm_energy::storage
{

    PostgresMeasurementRepository::PostgresMeasurementRepository(
        const std::string &connectionString, std::size_t maxBufferSize)
        : connectionString_(connectionString), maxBufferSize_(maxBufferSize)
    {
        connect();
        std::cout << "[PostgresRepository] Successfully connected to TimescaleDB (buffer size = "
                  << maxBufferSize << ")\n";
    }

    PostgresMeasurementRepository::~PostgresMeasurementRepository()
    {
        flush();
        if (connection_ && connection_->is_open())
        {
            std::cout << "[PostgresRepository] Database connection closed\n";
        }
    }

    void PostgresMeasurementRepository::connect()
    {
        connection_ = std::make_unique<pqxx::connection>(connectionString_);
        if (!connection_->is_open())
            throw std::runtime_error("Failed to open connection to TimescaleDB");
    }

    bool PostgresMeasurementRepository::tryReconnect(int maxAttempts)
    {
        for (int attempt = 1; attempt <= maxAttempts; ++attempt)
        {
            try
            {
                std::cout << "[PostgresRepository] Reconnect attempt " << attempt << "/"
                          << maxAttempts << "...\n";
                connect();
                std::cout << "[PostgresRepository] Reconnected successfully\n";
                return true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "[PostgresRepository] Reconnect failed: " << e.what() << std::endl;
                if (attempt < maxAttempts)
                {
                    auto backoff = std::chrono::seconds(1 << (attempt - 1));
                    std::this_thread::sleep_for(backoff);
                }
            }
        }
        return false;
    }

    bool PostgresMeasurementRepository::save(const core::SensorReading &reading)
    {
        return saveBatch({reading}) == 1;
    }

    std::size_t PostgresMeasurementRepository::saveBatch(const core::ReadingsBatch &readings)
    {
        if (readings.empty())
            return 0;

        {
            std::lock_guard<std::mutex> lock(bufferMutex_);
            buffer_.insert(buffer_.end(), readings.begin(), readings.end());
        }

        if (buffer_.size() >= maxBufferSize_)
            flush();

        return readings.size();
    }

    void PostgresMeasurementRepository::flush()
    {
        std::vector<core::SensorReading> toFlush;
        {
            std::lock_guard<std::mutex> lock(bufferMutex_);
            if (buffer_.empty())
                return;
            toFlush = std::move(buffer_);
        }

        if (toFlush.empty())
            return;

        doFlush(toFlush);
    }

    std::vector<std::string> PostgresMeasurementRepository::getUnknownDeviceIds(
        const std::vector<core::SensorReading> &readings)
    {
        std::vector<std::string> deviceIds;
        std::unordered_set<std::string> seenInBatch;

        {
            std::lock_guard<std::mutex> lock(deviceCacheMutex_);

            for (const auto &reading : readings)
            {
                if (knownDeviceIds_.contains(reading.deviceId) ||
                    seenInBatch.contains(reading.deviceId))
                {
                    continue;
                }

                seenInBatch.insert(reading.deviceId);
                deviceIds.push_back(reading.deviceId);
            }
        }

        return deviceIds;
    }

    void PostgresMeasurementRepository::markDevicesKnown(
        const std::vector<std::string> &deviceIds)
    {
        std::lock_guard<std::mutex> lock(deviceCacheMutex_);

        for (const auto &deviceId : deviceIds)
        {
            knownDeviceIds_.insert(deviceId);
        }
    }

    void PostgresMeasurementRepository::doFlush(const std::vector<core::SensorReading> &readings)
    {
        if (readings.empty())
            return;

        try
        {
            if (!connection_ || !connection_->is_open())
            {
                if (!tryReconnect(3))
                    throw std::runtime_error("Cannot reconnect to database");
            }

            pqxx::work txn{*connection_};
            const auto unknownDeviceIds = getUnknownDeviceIds(readings);

            for (const auto &deviceId : unknownDeviceIds)
            {
                ensureDeviceExists(txn, deviceId);
            }

            {
                txn.exec(
                    R"(CREATE TEMP TABLE staging_sensor_readings
                    (
                        recorded_at TIMESTAMPTZ NOT NULL,
                        device_id TEXT NOT NULL,
                        sensor_type TEXT NOT NULL,
                        numeric_value DOUBLE PRECISION,
                        bool_value BOOLEAN,
                        unit TEXT
                    ) ON COMMIT DROP)");

                const std::vector<std::string> columns{
                    "recorded_at", "device_id", "sensor_type", "numeric_value", "bool_value",
                    "unit"};
                auto stream = pqxx::stream_to::table(txn, "staging_sensor_readings", columns);

                for (const auto &r : readings)
                {
                    const std::string ts = fmt::format("{:%Y-%m-%d %H:%M:%S%z}", r.timestamp);
                    const std::optional<bool> boolVal = r.boolValue;

                    stream.write_values(ts, r.deviceId, r.sensorType, r.value, boolVal, r.unit);
                }

                stream.complete();
            }

            const auto insertResult = txn.exec(
                R"(INSERT INTO sensor_readings
                    (recorded_at, device_id, sensor_type, numeric_value, bool_value, unit)
                   SELECT recorded_at, device_id, sensor_type, numeric_value, bool_value, unit
                   FROM staging_sensor_readings
                   ON CONFLICT (recorded_at, device_id, sensor_type)
                   DO NOTHING)");

            txn.commit();
            markDevicesKnown(unknownDeviceIds);

            std::cout << fmt::format("[Postgres] Flushed {} readings ({} inserted)\n",
                                     readings.size(), insertResult.affected_rows());
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Postgres] Critical flush error: " << e.what() << std::endl;

            std::lock_guard<std::mutex> lock(bufferMutex_);
            buffer_.insert(buffer_.begin(), readings.begin(), readings.end());
        }
    }

    bool PostgresMeasurementRepository::saveAnomaly(const core::SensorReading &reading,
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
            (recorded_at, device_id, sensor_type, numeric_value, bool_value, unit,
             anomaly_type, severity, description, score)
           VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10))",
                pqxx::params{ts, reading.deviceId, reading.sensorType, reading.value, boolVal,
                             reading.unit, anomalyType, core::toString(severity), description,
                             score});

            txn.commit();

            std::cout << fmt::format("[Postgres] Anomaly saved: {} - {} ({})\n", anomalyType,
                                     reading.deviceId, core::toString(severity));

            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[Postgres] Save anomaly error: " << e.what() << std::endl;
            return false;
        }
    }

    void PostgresMeasurementRepository::ensureDeviceExists(pqxx::work &txn,
                                                           const std::string &deviceId)
    {
        txn.exec(
            R"(
            WITH selected_organization AS (
                SELECT organization_id AS id
                FROM users
                WHERE role = 'ADMIN'
                  AND organization_id IS NOT NULL
                ORDER BY id
                LIMIT 1
            ),
            inserted_organization AS (
                INSERT INTO organizations (name, type)
                SELECT
                    'MQTT Auto Workspace',
                    'BUSINESS'
                WHERE NOT EXISTS (
                    SELECT 1
                    FROM selected_organization
                )
                AND NOT EXISTS (
                    SELECT 1
                    FROM organizations
                )
                RETURNING id
            ),
            fallback_organization AS (
                SELECT id
                FROM selected_organization
                UNION ALL
                SELECT id
                FROM inserted_organization
                UNION ALL
                SELECT id
                FROM organizations
                ORDER BY id
                LIMIT 1
            ),
            inserted_building AS (
                INSERT INTO buildings (name, address, description, organization_id)
                SELECT
                    'MQTT Auto Devices',
                    'Automatically created from MQTT telemetry',
                    'Fallback catalog entry for live telemetry devices',
                    org.id
                FROM fallback_organization org
                WHERE NOT EXISTS (
                    SELECT 1
                    FROM buildings existing_building
                    WHERE existing_building.name = 'MQTT Auto Devices'
                      AND existing_building.organization_id = org.id
                )
                RETURNING id
            ),
            selected_building AS (
                SELECT id
                FROM inserted_building
                UNION ALL
                SELECT buildings.id
                FROM buildings
                JOIN fallback_organization org
                    ON org.id = buildings.organization_id
                WHERE buildings.name = 'MQTT Auto Devices'
                ORDER BY id
                LIMIT 1
            ),
            inserted_room AS (
                INSERT INTO rooms (room_name, room_type, floor_number, building_id)
                SELECT
                    $2,
                    'Telemetry',
                    0,
                    id
                FROM selected_building
                WHERE NOT EXISTS (
                    SELECT 1
                    FROM rooms
                    WHERE room_name = $2
                      AND building_id = selected_building.id
                )
                RETURNING id
            ),
            selected_room AS (
                SELECT id
                FROM inserted_room
                UNION ALL
                SELECT rooms.id
                FROM rooms
                JOIN selected_building
                    ON selected_building.id = rooms.building_id
                WHERE rooms.room_name = $2
                ORDER BY id
                LIMIT 1
            )
            INSERT INTO devices
                (device_id, device_name, device_model, firmware_version, room_id, is_online, last_seen_at)
            SELECT
                $1,
                $1,
                'MQTT virtual device',
                'auto',
                id,
                TRUE,
                NOW()
            FROM selected_room
            ON CONFLICT (device_id)
            DO UPDATE SET
                room_id = EXCLUDED.room_id,
                is_online = TRUE,
                last_seen_at = NOW()
            )",
            pqxx::params{deviceId, deviceId});
    }

} // namespace dorm_energy::storage

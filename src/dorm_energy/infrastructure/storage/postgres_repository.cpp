#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"
#include "dorm_energy/core/alert_severity.hpp"

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <pqxx/params>
#include <pqxx/stream_to>
#include <spdlog/spdlog.h>
#include <thread>
#include <unordered_set>

namespace dorm_energy::storage
{

    PostgresMeasurementRepository::PostgresMeasurementRepository(
        const std::string &connectionString,
        std::size_t maxBufferSize)
        : connectionString_(connectionString),
          maxBufferSize_(maxBufferSize)
    {
        connect();
        spdlog::info("[PostgresRepository] Successfully connected to TimescaleDB (buffer size = {})",
                     maxBufferSize);
    }

    PostgresMeasurementRepository::~PostgresMeasurementRepository()
    {
        flush();
        if (connection_ && connection_->is_open())
        {
            spdlog::info("[PostgresRepository] Database connection closed");
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
                spdlog::info("[PostgresRepository] Reconnect attempt {}/{}...", attempt,
                             maxAttempts);
                connect();
                spdlog::info("[PostgresRepository] Reconnected successfully");
                return true;
            }
            catch (const std::exception &e)
            {
                spdlog::error("[PostgresRepository] Reconnect failed: {}", e.what());
                if (attempt < maxAttempts)
                {
                    auto backoff = std::chrono::seconds(1 << (attempt - 1));
                    std::this_thread::sleep_for(backoff);
                }
            }
        }
        return false;
    }

    bool PostgresMeasurementRepository::saveReading(const core::SensorReading &reading)
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
                if (knownDeviceIds_.contains(reading.deviceId) || seenInBatch.contains(reading.deviceId))
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

    void PostgresMeasurementRepository::doFlush(
        const std::vector<core::SensorReading> &readings)
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

                const std::vector<std::string> columns{"recorded_at", "device_id", "sensor_type", "numeric_value", "bool_value", "unit"};
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

            spdlog::info("[Postgres] Flushed {} readings ({} inserted)", readings.size(),
                         insertResult.affected_rows());
        }
        catch (const std::exception &e)
        {
            spdlog::error("[Postgres] Critical flush error: {}", e.what());

            std::lock_guard<std::mutex> lock(bufferMutex_);
            buffer_.insert(buffer_.begin(), readings.begin(), readings.end());
        }
    }

    void PostgresMeasurementRepository::ensureDeviceExists(
        pqxx::work &txn,
        const std::string &deviceId)
    {
        const auto result = txn.exec_params(
            R"(
            WITH admin_org AS (
                SELECT organization_id AS id
                FROM users
                WHERE email = 'admin@dorm.energy'
                   OR username = 'admin'
                ORDER BY
                    CASE WHEN email = 'admin@dorm.energy' THEN 0 ELSE 1 END
                LIMIT 1
            ),
            inserted_building AS (
                INSERT INTO buildings (organization_id, name, address, description)
                SELECT id, 'MQTT Auto Devices', 'MQTT telemetry', 'Automatically created from MQTT telemetry'
                FROM admin_org
                WHERE NOT EXISTS (
                    SELECT 1
                    FROM buildings
                    WHERE organization_id = admin_org.id
                      AND name = 'MQTT Auto Devices'
                )
                RETURNING id
            ),
            target_building AS (
                SELECT id FROM inserted_building
                UNION ALL
                SELECT b.id
                FROM buildings b
                JOIN admin_org ao
                    ON ao.id = b.organization_id
                WHERE b.name = 'MQTT Auto Devices'
                LIMIT 1
            ),
            inserted_room AS (
                INSERT INTO rooms (building_id, room_name, room_type, floor_number)
                SELECT id, 'Auto-discovered devices', 'MQTT', 0
                FROM target_building
                WHERE NOT EXISTS (
                    SELECT 1
                    FROM rooms
                    WHERE building_id = target_building.id
                      AND room_name = 'Auto-discovered devices'
                )
                RETURNING id
            ),
            target_room AS (
                SELECT id FROM inserted_room
                UNION ALL
                SELECT r.id
                FROM rooms r
                JOIN target_building tb
                    ON tb.id = r.building_id
                WHERE r.room_name = 'Auto-discovered devices'
                LIMIT 1
            )
            INSERT INTO devices
                (device_id, device_name, device_model, firmware_version, room_id, is_online, last_seen_at)
            SELECT
                $1, $1, 'ESP32', 'auto-discovered', id, TRUE, NOW()
            FROM target_room
            ON CONFLICT (device_id)
            DO UPDATE SET
                is_online = TRUE,
                last_seen_at = NOW()
            )",
            deviceId);

        if (result.affected_rows() == 0)
        {
            throw std::runtime_error("Cannot auto-discover device because admin workspace is missing");
        }
    }

} // namespace dorm_energy::storage

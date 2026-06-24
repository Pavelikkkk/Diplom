#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"

#include <stdexcept>

namespace dorm_energy::storage
{

    std::vector<storage::DeviceDto> PostgresMeasurementRepository::getDevices(
        int organizationId)
    {
        std::vector<DeviceDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
            d.device_id,
            COALESCE(NULLIF(d.device_name, ''), d.device_id) AS device_name,
            COALESCE(NULLIF(d.device_model, ''), 'Unknown model') AS device_model,
            COALESCE(NULLIF(d.firmware_version, ''), 'Unknown firmware') AS firmware_version,
            ('devices/' || d.device_id || '/+') AS mqtt_topic,

            r.id AS room_id,
            COALESCE(NULLIF(r.room_name, ''), 'Unassigned room') AS room_name,

            b.id AS building_id,

            o.id AS organization_id,

            (d.last_seen_at IS NOT NULL AND d.last_seen_at >= NOW() - INTERVAL '2 minutes') AS is_online,

            COALESCE(
                d.last_seen_at::text,
                ''
            ) AS last_seen_at

        FROM devices d

        LEFT JOIN rooms r
            ON r.id = d.room_id

        LEFT JOIN buildings b
            ON b.id = r.building_id

        LEFT JOIN organizations o
            ON o.id = b.organization_id
        WHERE ($1 = 0 OR b.organization_id = $1)
           OR ($1 = 0 AND d.room_id IS NULL)
        ORDER BY
            o.name NULLS LAST,
            b.name NULLS LAST,
            r.room_name NULLS LAST,
            d.device_id
            )",
            organizationId);

        for (auto const &row : rows)
        {
            DeviceDto dto;

            dto.deviceId = row["device_id"].c_str();
            dto.deviceName = row["device_name"].c_str();
            dto.deviceModel = row["device_model"].c_str();
            dto.firmwareVersion = row["firmware_version"].c_str();
            dto.mqttTopic = row["mqtt_topic"].c_str();
            dto.roomName = row["room_name"].c_str();
            dto.roomId = row["room_id"].as<int>(0);
            dto.buildingId = row["building_id"].as<int>(0);
            dto.organizationId = row["organization_id"].as<int>(0);
            dto.isOnline = row["is_online"].as<bool>(false);
            dto.lastSeenAt = row["last_seen_at"].c_str();

            result.push_back(dto);
        }

        return result;
    }

    std::vector<storage::BuildingDto> PostgresMeasurementRepository::getBuildings(
        int organizationId)
    {
        std::vector<BuildingDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                id,
                name,
                COALESCE(address, '') AS address,
                COALESCE(description, '') AS description
            FROM buildings
            WHERE ($1 = 0 OR organization_id = $1)
            ORDER BY name
            )",
            organizationId);

        for (auto const &row : rows)
        {
            BuildingDto dto;

            dto.id = row["id"].as<int>();
            dto.name = row["name"].c_str();
            dto.address = row["address"].c_str();
            dto.description = row["description"].c_str();

            result.push_back(dto);
        }

        return result;
    }

    std::vector<storage::RoomDto> PostgresMeasurementRepository::getRooms(
        int organizationId)
    {
        std::vector<RoomDto> result;

        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                r.id,
                r.building_id,
                r.room_name,
                r.room_type,
                r.floor_number,
                r.min_normal_power_kw,
                r.max_normal_power_kw,
                r.allow_unattended_power,
                COALESCE(latest_motion.bool_value, FALSE) AS motion,
                COALESCE(latest_power.numeric_value, 0.0) AS power,
                COALESCE(latest_light.numeric_value, 0.0) AS light
            FROM rooms r
            JOIN buildings b
                ON b.id = r.building_id
            LEFT JOIN LATERAL (
                SELECT sr.bool_value
                FROM sensor_readings sr
                JOIN devices d
                    ON d.device_id = sr.device_id
                WHERE d.room_id = r.id
                  AND sr.sensor_type = 'motion'
                  AND sr.bool_value IS NOT NULL
                ORDER BY sr.recorded_at DESC
                LIMIT 1
            ) latest_motion ON TRUE
            LEFT JOIN LATERAL (
                SELECT
                    CASE
                        WHEN sr.unit = 'kW' THEN sr.numeric_value * 1000.0
                        ELSE sr.numeric_value
                    END AS numeric_value
                FROM sensor_readings sr
                JOIN devices d
                    ON d.device_id = sr.device_id
                WHERE d.room_id = r.id
                  AND sr.sensor_type = 'power'
                  AND sr.numeric_value IS NOT NULL
                ORDER BY sr.recorded_at DESC
                LIMIT 1
            ) latest_power ON TRUE
            LEFT JOIN LATERAL (
                SELECT sr.numeric_value
                FROM sensor_readings sr
                JOIN devices d
                    ON d.device_id = sr.device_id
                WHERE d.room_id = r.id
                  AND sr.sensor_type = 'light'
                  AND sr.numeric_value IS NOT NULL
                ORDER BY sr.recorded_at DESC
                LIMIT 1
            ) latest_light ON TRUE
            WHERE ($1 = 0 OR b.organization_id = $1)
            ORDER BY room_name
            )",
            organizationId);

        for (const auto &row : rows)
        {
            RoomDto dto;

            dto.id = row["id"].as<int>();
            dto.buildingId = row["building_id"].as<int>();
            dto.roomName = row["room_name"].c_str();
            dto.roomType = row["room_type"].c_str();
            dto.floorNumber = row["floor_number"].as<int>();
            dto.minNormalPowerKw = row["min_normal_power_kw"].as<double>(0.0);
            dto.maxNormalPowerKw = row["max_normal_power_kw"].as<double>(2.8);
            dto.allowUnattendedPower = row["allow_unattended_power"].as<bool>(false);
            dto.motion = row["motion"].as<bool>(false);
            dto.power = row["power"].as<double>(0.0);
            dto.light = row["light"].as<double>(0.0);

            result.push_back(dto);
        }

        return result;
    }

    std::optional<RoomDetectionProfileDto> PostgresMeasurementRepository::getRoomDetectionProfileByDeviceId(
        const std::string &deviceId)
    {
        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                r.min_normal_power_kw,
                r.max_normal_power_kw,
                r.allow_unattended_power,
                COALESCE(AVG(sr.numeric_value), 0.0) AS baseline_average_power_kw,
                COUNT(sr.numeric_value) AS baseline_power_sample_count
            FROM devices d
            JOIN rooms r
                ON r.id = d.room_id
            LEFT JOIN sensor_readings sr
                ON sr.device_id = d.device_id
                AND sr.sensor_type = 'power'
                AND sr.unit = 'kW'
                AND sr.numeric_value IS NOT NULL
                AND sr.recorded_at >= NOW() - INTERVAL '7 days'
            WHERE d.device_id = $1
            GROUP BY
                r.min_normal_power_kw,
                r.max_normal_power_kw,
                r.allow_unattended_power
            LIMIT 1
            )",
            deviceId);

        if (rows.empty())
        {
            return std::nullopt;
        }

        const auto &row = rows[0];

        return RoomDetectionProfileDto{
            .minNormalPowerKw = row["min_normal_power_kw"].as<double>(0.0),
            .maxNormalPowerKw = row["max_normal_power_kw"].as<double>(2.8),
            .allowUnattendedPower = row["allow_unattended_power"].as<bool>(false),
            .baselineAveragePowerKw = row["baseline_average_power_kw"].as<double>(0.0),
            .baselinePowerSampleCount = row["baseline_power_sample_count"].as<int>(0),
            .baselineReady = row["baseline_power_sample_count"].as<int>(0) > 0,
        };
    }

    int PostgresMeasurementRepository::createBuildingForOrganization(
        int organizationId,
        const std::string &name,
        const std::string &address,
        const std::string &description)
    {
        pqxx::work txn(*connection_);

        if (name.empty())
        {
            throw std::runtime_error("Building name is required");
        }

        if (address.empty())
        {
            throw std::runtime_error("Building address is required");
        }

        const auto duplicates = txn.exec_params(
            R"(
            SELECT id
            FROM buildings
            WHERE organization_id = $1
              AND LOWER(TRIM(name)) = LOWER(TRIM($2))
              AND LOWER(TRIM(COALESCE(address, ''))) = LOWER(TRIM($3))
            LIMIT 1
            )",
            organizationId, name, address);

        if (!duplicates.empty())
        {
            throw std::runtime_error("A building with this name and address already exists");
        }

        auto row = txn.exec_params1(
            R"(
            INSERT INTO buildings
            (
                organization_id,
                name,
                address,
                description
            )
            VALUES
            (
                $1,
                $2,
                $3,
                $4
            )
            RETURNING id
            )",
            organizationId, name, address, description);

        txn.commit();

        return row["id"].as<int>();
    }

    int PostgresMeasurementRepository::createRoomForBuilding(
        int buildingId,
        const std::string &roomName,
        const std::string &roomType,
        int floorNumber,
        double minNormalPowerKw,
        double maxNormalPowerKw,
        bool allowUnattendedPower)
    {
        pqxx::work txn(*connection_);

        auto row = txn.exec_params1(
            R"(
            INSERT INTO rooms
            (
                building_id,
                room_name,
                room_type,
                floor_number,
                min_normal_power_kw,
                max_normal_power_kw,
                allow_unattended_power
            )
            VALUES
            (
                $1,
                $2,
                $3,
                $4,
                $5,
                $6,
                $7
            )
            RETURNING id
            )",
            buildingId, roomName, roomType, floorNumber, minNormalPowerKw, maxNormalPowerKw, allowUnattendedPower);

        txn.commit();

        return row["id"].as<int>();
    }

    bool PostgresMeasurementRepository::createDeviceForRoom(
        const std::string &deviceId,
        const std::string &deviceName,
        const std::string &deviceModel,
        const std::string &firmwareVersion,
        int roomId)
    {
        pqxx::work txn(*connection_);

        if (roomId <= 0)
        {
            throw std::runtime_error("Device room is required");
        }

        txn.exec_params(
            R"(
            INSERT INTO devices
            (
                device_id,
                device_name,
                device_model,
                firmware_version,
                room_id,
                is_online,
                last_seen_at
            )
            VALUES
            (
                $1,
                $2,
                $3,
                $4,
                $5,
                FALSE,
                NULL
            )
            ON CONFLICT (device_id)
            DO UPDATE SET
                device_name = EXCLUDED.device_name,
                device_model = EXCLUDED.device_model,
                firmware_version = EXCLUDED.firmware_version,
                room_id = EXCLUDED.room_id
            )",
            deviceId, deviceName, deviceModel, firmwareVersion, roomId);

        txn.commit();

        return true;
    }

    bool PostgresMeasurementRepository::updateBuilding(
        int buildingId,
        int organizationId,
        const std::string &name,
        const std::string &address,
        const std::string &description)
    {
        pqxx::work txn(*connection_);

        if (name.empty())
        {
            throw std::runtime_error("Building name is required");
        }

        if (address.empty())
        {
            throw std::runtime_error("Building address is required");
        }

        const auto duplicates = txn.exec_params(
            R"(
            SELECT id
            FROM buildings
            WHERE id <> $1
              AND organization_id = $2
              AND LOWER(TRIM(name)) = LOWER(TRIM($3))
              AND LOWER(TRIM(COALESCE(address, ''))) = LOWER(TRIM($4))
            LIMIT 1
            )",
            buildingId, organizationId, name, address);

        if (!duplicates.empty())
        {
            throw std::runtime_error("A building with this name and address already exists");
        }

        const auto result = txn.exec_params(
            R"(
            UPDATE buildings
            SET
                organization_id = $2,
                name = $3,
                address = $4,
                description = $5
            WHERE id = $1
            )",
            buildingId, organizationId, name, address, description);

        txn.commit();

        return result.affected_rows() > 0;
    }

    bool PostgresMeasurementRepository::deleteBuilding(
        int buildingId)
    {
        pqxx::work txn(*connection_);

        const auto result = txn.exec_params(
            "DELETE FROM buildings WHERE id = $1",
            buildingId);

        txn.commit();

        return result.affected_rows() > 0;
    }

    bool PostgresMeasurementRepository::updateRoom(
        int roomId,
        int buildingId,
        const std::string &roomName,
        const std::string &roomType,
        int floorNumber,
        double minNormalPowerKw,
        double maxNormalPowerKw,
        bool allowUnattendedPower)
    {
        pqxx::work txn(*connection_);

        if (roomId <= 0)
        {
            throw std::runtime_error("Device room is required");
        }

        const auto result = txn.exec_params(
            R"(
            UPDATE rooms
            SET
                building_id = $2,
                room_name = $3,
                room_type = $4,
                floor_number = $5,
                min_normal_power_kw = $6,
                max_normal_power_kw = $7,
                allow_unattended_power = $8
            WHERE id = $1
            )",
            roomId, buildingId, roomName, roomType, floorNumber, minNormalPowerKw, maxNormalPowerKw, allowUnattendedPower);

        txn.commit();

        return result.affected_rows() > 0;
    }

    bool PostgresMeasurementRepository::deleteRoom(
        int roomId)
    {
        pqxx::work txn(*connection_);

        const auto result = txn.exec_params(
            "DELETE FROM rooms WHERE id = $1",
            roomId);

        txn.commit();

        return result.affected_rows() > 0;
    }

    bool PostgresMeasurementRepository::updateDevice(
        const std::string &deviceId,
        const std::string &deviceName,
        const std::string &deviceModel,
        const std::string &firmwareVersion,
        int roomId)
    {
        pqxx::work txn(*connection_);

        const auto result = txn.exec_params(
            R"(
            UPDATE devices
            SET
                device_name = $2,
                device_model = $3,
                firmware_version = $4,
                room_id = $5
            WHERE device_id = $1
            )",
            deviceId, deviceName, deviceModel, firmwareVersion, roomId);

        txn.commit();

        return result.affected_rows() > 0;
    }

    bool PostgresMeasurementRepository::deleteDevice(
        const std::string &deviceId)
    {
        pqxx::work txn(*connection_);

        const auto result = txn.exec_params(
            "DELETE FROM devices WHERE device_id = $1",
            deviceId);

        txn.commit();

        return result.affected_rows() > 0;
    }

} // namespace dorm_energy::storage

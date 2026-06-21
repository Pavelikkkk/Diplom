#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"

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
            d.device_name,
            d.device_model,
            d.firmware_version,

            r.id AS room_id,
            r.room_name,

            b.id AS building_id,

            o.id AS organization_id,

            d.is_online,

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
            )",
            organizationId);

        for (auto const &row : rows)
        {
            DeviceDto dto;

            dto.deviceId = row["device_id"].c_str();
            dto.deviceName = row["device_name"].c_str();
            dto.deviceModel = row["device_model"].c_str();
            dto.firmwareVersion = row["firmware_version"].c_str();
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
                r.floor_number
            FROM rooms r
            JOIN buildings b
                ON b.id = r.building_id
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

            result.push_back(dto);
        }

        return result;
    }

    int PostgresMeasurementRepository::createBuildingForOrganization(
        int organizationId,
        const std::string &name,
        const std::string &address,
        const std::string &description)
    {
        pqxx::work txn(*connection_);

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
        int floorNumber)
    {
        pqxx::work txn(*connection_);

        auto row = txn.exec_params1(
            R"(
            INSERT INTO rooms
            (
                building_id,
                room_name,
                room_type,
                floor_number
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
            buildingId, roomName, roomType, floorNumber);

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
                TRUE,
                NOW()
            )
            ON CONFLICT (device_id)
            DO UPDATE SET
                device_name = EXCLUDED.device_name,
                device_model = EXCLUDED.device_model,
                firmware_version = EXCLUDED.firmware_version,
                room_id = EXCLUDED.room_id,
                is_online = TRUE,
                last_seen_at = NOW()
            )",
            deviceId, deviceName, deviceModel, firmwareVersion, roomId);

        txn.commit();

        return true;
    }

} // namespace dorm_energy::storage

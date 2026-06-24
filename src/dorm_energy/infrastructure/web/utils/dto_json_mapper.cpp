#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"

namespace dorm_energy::web
{
    Json::Value toJson(const storage::AnomalyDto &dto)
    {
        Json::Value json;
        json["room"] = dto.room;
        json["type"] = dto.type;
        json["severity"] = dto.severity;
        json["score"] = dto.score;
        json["description"] = dto.description;
        json["detectedAt"] = dto.detectedAt;
        return json;
    }

    Json::Value toJson(const storage::AnomalyStatsDto &dto)
    {
        Json::Value json;
        json["type"] = dto.type;
        json["count"] = dto.count;
        return json;
    }

    Json::Value toJson(const storage::BuildingDto &dto)
    {
        Json::Value json;
        json["id"] = dto.id;
        json["name"] = dto.name;
        json["address"] = dto.address;
        json["description"] = dto.description;
        return json;
    }

    Json::Value toJson(const storage::DeviceDto &dto)
    {
        Json::Value json;
        json["deviceId"] = dto.deviceId;
        json["deviceName"] = dto.deviceName;
        json["deviceModel"] = dto.deviceModel;
        json["firmwareVersion"] = dto.firmwareVersion;
        json["roomName"] = dto.roomName;
        json["mqttTopic"] = dto.mqttTopic;
        json["roomId"] = dto.roomId;
        json["buildingId"] = dto.buildingId;
        json["organizationId"] = dto.organizationId;
        json["isOnline"] = dto.isOnline;
        json["lastSeenAt"] = dto.lastSeenAt;
        return json;
    }

    Json::Value toJson(const storage::EnergyByRoomDto &dto)
    {
        Json::Value json;
        json["roomName"] = dto.roomName;
        json["power"] = dto.avgPower;
        return json;
    }

    Json::Value toJson(const storage::PowerPointDto &dto)
    {
        Json::Value json;
        json["time"] = dto.time;
        json["power"] = dto.power;
        return json;
    }

    Json::Value toJson(const storage::RoomDto &dto)
    {
        Json::Value json;
        json["id"] = dto.id;
        json["buildingId"] = dto.buildingId;
        json["roomName"] = dto.roomName;
        json["roomType"] = dto.roomType;
        json["floorNumber"] = dto.floorNumber;
        json["minNormalPowerKw"] = dto.minNormalPowerKw;
        json["maxNormalPowerKw"] = dto.maxNormalPowerKw;
        json["allowUnattendedPower"] = dto.allowUnattendedPower;
        json["motion"] = dto.motion;
        json["power"] = dto.power;
        json["light"] = dto.light;
        return json;
    }

    Json::Value toJson(const storage::SeverityStatsDto &dto)
    {
        Json::Value json;
        json["severity"] = dto.severity;
        json["count"] = dto.count;
        return json;
    }

    Json::Value toJson(const storage::SubscriptionDto &dto)
    {
        Json::Value json;
        json["plan"] = dto.plan;
        json["status"] = dto.status;
        json["maxBuildings"] = dto.maxBuildings;
        json["maxRooms"] = dto.maxRooms;
        json["maxDevices"] = dto.maxDevices;
        return json;
    }

    Json::Value toJson(const storage::TopConsumerDto &dto)
    {
        Json::Value json;
        json["roomName"] = dto.roomName;
        json["power"] = dto.power;
        return json;
    }

    Json::Value toJson(const UserDto &dto)
    {
        Json::Value json;
        json["id"] = dto.id;
        json["username"] = dto.username;
        json["email"] = dto.email;
        json["role"] = dto.role;
        json["organizationId"] = dto.organizationId;
        json["accountType"] = dto.accountType;
        json["telegramChatId"] = dto.telegramChatId;
        return json;
    }
} // namespace dorm_energy::web

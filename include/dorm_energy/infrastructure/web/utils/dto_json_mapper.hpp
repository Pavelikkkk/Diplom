#pragma once

#include "dorm_energy/domain/storage/dto/anomaly_dto.hpp"
#include "dorm_energy/domain/storage/dto/anomaly_stats_dto.hpp"
#include "dorm_energy/domain/storage/dto/building_dto.hpp"
#include "dorm_energy/domain/storage/dto/device_dto.hpp"
#include "dorm_energy/domain/storage/dto/energy_by_room_dto.hpp"
#include "dorm_energy/domain/storage/dto/power_point_dto.hpp"
#include "dorm_energy/domain/storage/dto/room_dto.hpp"
#include "dorm_energy/domain/storage/dto/severity_stats_dto.hpp"
#include "dorm_energy/domain/storage/dto/subscription_dto.hpp"
#include "dorm_energy/domain/storage/dto/top_consumer_dto.hpp"
#include "dorm_energy/domain/storage/dto/user_dto.hpp"

#include <json/json.h>
#include <vector>

namespace dorm_energy::web
{
    Json::Value toJson(const storage::AnomalyDto &dto);
    Json::Value toJson(const storage::AnomalyStatsDto &dto);
    Json::Value toJson(const storage::BuildingDto &dto);
    Json::Value toJson(const storage::DeviceDto &dto);
    Json::Value toJson(const storage::EnergyByRoomDto &dto);
    Json::Value toJson(const storage::PowerPointDto &dto);
    Json::Value toJson(const storage::RoomDto &dto);
    Json::Value toJson(const storage::SeverityStatsDto &dto);
    Json::Value toJson(const storage::SubscriptionDto &dto);
    Json::Value toJson(const storage::TopConsumerDto &dto);
    Json::Value toJson(const UserDto &dto);

    template <typename T>
    Json::Value toJsonArray(const std::vector<T> &items)
    {
        Json::Value json(Json::arrayValue);

        for (const auto &item : items)
        {
            json.append(toJson(item));
        }

        return json;
    }
} // namespace dorm_energy::web

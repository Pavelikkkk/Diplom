#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"

namespace dorm_energy::storage
{

    Json::Value PostgresMeasurementRepository::getAdminOverview()
    {
        // Открываем транзакцию PostgreSQL.
        // pqxx::work нужен для выполнения SQL-запросов в рамках одной транзакции.
        pqxx::work txn(*connection_);

        Json::Value json;
        json["users"] = Json::Value(Json::arrayValue);
        json["buildings"] = Json::Value(Json::arrayValue);
        json["rooms"] = Json::Value(Json::arrayValue);
        json["devices"] = Json::Value(Json::arrayValue);

        auto users = txn.exec(
            R"(
            SELECT
                u.id,
                u.username,
                u.email,
                u.role,
                COALESCE(u.account_type, 'PERSONAL') AS account_type,
                COALESCE(u.organization_id, 0) AS organization_id,
                COALESCE(o.name, '') AS organization_name,
                COALESCE(s.plan, 'STANDARD') AS plan,
                COUNT(DISTINCT b.id) AS buildings_count,
                COUNT(DISTINCT r.id) AS rooms_count,
                COUNT(DISTINCT d.device_id) AS devices_count
            FROM users u
            LEFT JOIN organizations o
                ON o.id = u.organization_id
            LEFT JOIN subscriptions s
                ON s.user_id = u.id
            LEFT JOIN buildings b
                ON b.organization_id = u.organization_id
            LEFT JOIN rooms r
                ON r.building_id = b.id
            LEFT JOIN devices d
                ON d.room_id = r.id
            GROUP BY
                u.id,
                u.username,
                u.email,
                u.role,
                u.account_type,
                u.organization_id,
                o.name,
                s.plan
            ORDER BY u.id
            )");

        for (const auto &row : users)
        {
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["username"] = row["username"].c_str();
            item["email"] = row["email"].c_str();
            item["role"] = row["role"].c_str();
            item["accountType"] = row["account_type"].c_str();
            item["organizationId"] = row["organization_id"].as<int>(0);
            item["organizationName"] = row["organization_name"].c_str();
            item["plan"] = row["plan"].c_str();
            item["buildingsCount"] = row["buildings_count"].as<int>(0);
            item["roomsCount"] = row["rooms_count"].as<int>(0);
            item["devicesCount"] = row["devices_count"].as<int>(0);

            json["users"].append(item);
        }

        auto buildings = txn.exec(
            R"(
            SELECT
                id,
                organization_id,
                name,
                COALESCE(address, '') AS address,
                COALESCE(description, '') AS description
            FROM buildings
            ORDER BY organization_id, name
            )");

        for (const auto &row : buildings)
        {
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["organizationId"] = row["organization_id"].as<int>(0);
            item["name"] = row["name"].c_str();
            item["address"] = row["address"].c_str();
            item["description"] = row["description"].c_str();

            json["buildings"].append(item);
        }

        auto rooms = txn.exec(
            R"(
            SELECT
                id,
                building_id,
                room_name,
                room_type,
                floor_number,
                min_normal_power_kw,
                max_normal_power_kw,
                allow_unattended_power
            FROM rooms
            ORDER BY building_id, room_name
            )");

        for (const auto &row : rooms)
        {
            Json::Value item;
            item["id"] = row["id"].as<int>();
            item["buildingId"] = row["building_id"].as<int>();
            item["roomName"] = row["room_name"].c_str();
            item["roomType"] = row["room_type"].c_str();
            item["floorNumber"] = row["floor_number"].as<int>();
            item["minNormalPowerKw"] = row["min_normal_power_kw"].as<double>(0.0);
            item["maxNormalPowerKw"] = row["max_normal_power_kw"].as<double>(2.8);
            item["allowUnattendedPower"] = row["allow_unattended_power"].as<bool>(false);

            json["rooms"].append(item);
        }

        auto devices = txn.exec(
            R"(
            SELECT
                d.device_id,
                COALESCE(NULLIF(d.device_name, ''), d.device_id) AS device_name,
                COALESCE(NULLIF(d.device_model, ''), 'Unknown model') AS device_model,
                COALESCE(NULLIF(d.firmware_version, ''), 'Unknown firmware') AS firmware_version,
                COALESCE(d.room_id, 0) AS room_id,
                COALESCE(r.room_name, '') AS room_name,
                COALESCE(b.id, 0) AS building_id,
                COALESCE(b.organization_id, 0) AS organization_id,
                (d.last_seen_at IS NOT NULL AND d.last_seen_at >= NOW() - INTERVAL '2 minutes') AS is_online,
                COALESCE(d.last_seen_at::text, '') AS last_seen_at,
                ('devices/' || d.device_id || '/+') AS mqtt_topic
            FROM devices d
            LEFT JOIN rooms r
                ON r.id = d.room_id
            LEFT JOIN buildings b
                ON b.id = r.building_id
            ORDER BY
                b.organization_id NULLS LAST,
                b.name NULLS LAST,
                r.room_name NULLS LAST,
                d.device_id
            )");

        for (const auto &row : devices)
        {
            Json::Value item;
            item["deviceId"] = row["device_id"].c_str();
            item["deviceName"] = row["device_name"].c_str();
            item["deviceModel"] = row["device_model"].c_str();
            item["firmwareVersion"] = row["firmware_version"].c_str();
            item["roomId"] = row["room_id"].as<int>(0);
            item["roomName"] = row["room_name"].c_str();
            item["buildingId"] = row["building_id"].as<int>(0);
            item["organizationId"] = row["organization_id"].as<int>(0);
            item["isOnline"] = row["is_online"].as<bool>(false);
            item["lastSeenAt"] = row["last_seen_at"].c_str();
            item["mqttTopic"] = row["mqtt_topic"].c_str();

            json["devices"].append(item);
        }

        return json;
    }

} // namespace dorm_energy::storage

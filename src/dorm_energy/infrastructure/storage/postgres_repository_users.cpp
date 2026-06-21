#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"

#include <spdlog/spdlog.h>

namespace dorm_energy::storage
{

    std::optional<UserDto> PostgresMeasurementRepository::findUserByEmail(
        const std::string &email)
    {
        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                id,
                username,
                email,
                password_hash,
                role,
                organization_id,
                account_type,
                COALESCE(telegram_chat_id, '') AS telegram_chat_id
            FROM users
            WHERE email = $1
            )",
            email);

        if (rows.empty())
        {
            return std::nullopt;
        }

        const auto &row = rows[0];

        UserDto dto;

        dto.id = row["id"].as<int>();
        dto.username = row["username"].c_str();
        dto.email = row["email"].c_str();
        dto.passwordHash = row["password_hash"].c_str();
        dto.role = row["role"].c_str();
        dto.organizationId = row["organization_id"].is_null() ? 0 : row["organization_id"].as<int>();
        dto.accountType = row["account_type"].c_str();
        dto.telegramChatId = row["telegram_chat_id"].c_str();

        return dto;
    }

    std::optional<UserDto> PostgresMeasurementRepository::findUserById(
        int userId)
    {
        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                id,
                username,
                email,
                password_hash,
                role,
                organization_id,
                account_type,
                COALESCE(telegram_chat_id, '') AS telegram_chat_id
            FROM users
            WHERE id = $1
            )",
            userId);

        if (rows.empty())
        {
            return std::nullopt;
        }

        const auto &row = rows[0];

        UserDto dto;

        dto.id = row["id"].as<int>();
        dto.username = row["username"].c_str();
        dto.email = row["email"].c_str();
        dto.passwordHash = row["password_hash"].c_str();
        dto.role = row["role"].c_str();
        dto.organizationId = row["organization_id"].is_null() ? 0 : row["organization_id"].as<int>();
        dto.accountType = row["account_type"].c_str();
        dto.telegramChatId = row["telegram_chat_id"].c_str();

        return dto;
    }

    int PostgresMeasurementRepository::createUser(
        const UserDto &user)
    {
        try
        {
            pqxx::work txn(*connection_);

            const auto organizationType = user.accountType == "BUSINESS" ? "BUSINESS" : "PERSONAL";

            auto organizationRow = txn.exec_params1(
                R"(
        INSERT INTO organizations
        (
            name,
            type
        )
        VALUES
        (
            $1,
            $2
        )
        RETURNING id
        )",
                user.username + " workspace", organizationType);

            const auto organizationId = organizationRow["id"].as<int>();

            auto row = txn.exec_params1(
                R"(
        INSERT INTO users
        (
            username,
            email,
            password_hash,
            role,
            account_type,
            organization_id
        )
        VALUES
        (
            $1,
            $2,
            $3,
            $4,
            $5,
            $6
        )
        RETURNING id
        )",
                user.username, user.email, user.passwordHash, user.role, user.accountType,
                organizationId);

            const auto userId = row["id"].as<int>();

            const auto isBusiness = user.accountType == "BUSINESS";

            txn.exec_params(
                R"(
        INSERT INTO subscriptions
        (
            organization_id,
            user_id,
            plan,
            status,
            max_buildings,
            max_rooms,
            max_devices
        )
        VALUES
        (
            $1,
            $2,
            $3,
            'ACTIVE',
            $4,
            $5,
            $6
        )
        )",
                organizationId, userId, isBusiness ? "BUSINESS" : "STANDARD", isBusiness ? 25 : 0,
                isBusiness ? 250 : 5, isBusiness ? 1200 : 20);

            txn.commit();

            return userId;
        }
        catch (const std::exception &ex)
        {
            spdlog::error("CREATE USER ERROR: {}", ex.what());

            throw;
        }
    }

    bool PostgresMeasurementRepository::updateUserTelegramChatId(
        int userId,
        const std::string &telegramChatId)
    {
        pqxx::work txn(*connection_);

        auto result = txn.exec_params(
            R"(
            UPDATE users
            SET telegram_chat_id = $2,
                updated_at = NOW()
            WHERE id = $1
            )",
            userId, telegramChatId);

        txn.commit();

        return result.affected_rows() == 1;
    }

    SubscriptionDto PostgresMeasurementRepository::getUserSubscription(
        int userId)
    {
        pqxx::work txn(*connection_);

        auto rows = txn.exec_params(
            R"(
            SELECT
                plan,
                status,
                max_buildings,
                max_rooms,
                max_devices
            FROM subscriptions
            WHERE user_id = $1
            ORDER BY created_at DESC
            LIMIT 1
            )",
            userId);

        if (rows.empty())
        {
            return SubscriptionDto{};
        }

        const auto &row = rows[0];

        return SubscriptionDto{
            .plan = row["plan"].c_str(),
            .status = row["status"].c_str(),
            .maxBuildings = row["max_buildings"].as<int>(0),
            .maxRooms = row["max_rooms"].as<int>(0),
            .maxDevices = row["max_devices"].as<int>(0),
        };
    }

} // namespace dorm_energy::storage

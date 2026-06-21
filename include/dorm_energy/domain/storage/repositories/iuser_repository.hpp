#pragma once

#include "dorm_energy/domain/storage/dto/user_dto.hpp"

#include <optional>
#include <string>

namespace dorm_energy::storage
{
    class IUserRepository
    {
    public:
        virtual ~IUserRepository() = default;

        virtual std::optional<UserDto> findUserByEmail(
            const std::string &email) = 0;

        virtual std::optional<UserDto> findUserById(
            int userId) = 0;

        virtual int createUser(
            const UserDto &user) = 0;

        virtual bool updateUserTelegramChatId(
            int userId,
            const std::string &telegramChatId) = 0;
    };
} // namespace dorm_energy::storage

#pragma once
#include <string>

struct UserDto
{
    int id;

    std::string username;

    std::string email;

    std::string passwordHash;

    std::string role;

    int organizationId;

    std::string accountType;

    std::string telegramChatId;
};

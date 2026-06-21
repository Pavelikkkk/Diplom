#pragma once

#include <string>

namespace dorm_energy::auth
{
    struct UserClaims
    {
        int userId{0};
        std::string email{};
        std::string role{};
    };
}
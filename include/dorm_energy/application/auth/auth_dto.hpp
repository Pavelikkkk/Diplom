#pragma once

#include <string>

namespace dorm_energy::auth
{
    struct LoginRequest
    {
        std::string email{};
        std::string password{};
    };

    struct RegisterRequest
    {
        std::string username{};
        std::string email{};
        std::string password{};
        std::string accountType{};
    };

    struct AuthResponse
    {
        std::string token{};
    };
}
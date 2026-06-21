#pragma once

#include <string>

namespace dorm_energy::application
{
    struct JwtConfig
    {
        std::string secret{};
        int tokenLifetimeHours{24};
    };
} // namespace dorm_energy::application

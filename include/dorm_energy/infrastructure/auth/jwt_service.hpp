#pragma once

#include "dorm_energy/domain/auth/ijwt_service.hpp"

#include <string>

namespace dorm_energy::auth
{
    class JwtService final : public IJwtService
    {
    public:
        explicit JwtService(
            std::string secret,
            int tokenLifetimeHours);

        std::string generateToken(
            int userId,
            const std::string &email,
            const std::string &role) override;

        UserClaims validateToken(
            const std::string &token) override;

    private:
        std::string secret_;
        int tokenLifetimeHours_;
    };
}

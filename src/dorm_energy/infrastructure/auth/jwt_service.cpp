#include "dorm_energy/infrastructure/auth/jwt_service.hpp"

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/nlohmann-json/traits.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <utility>

namespace dorm_energy::auth
{
    namespace
    {
        using JwtTraits = jwt::traits::nlohmann_json;

        constexpr const char *Issuer = "dorm-energy";
    }

    JwtService::JwtService(
        std::string secret,
        int tokenLifetimeHours)
        : secret_(std::move(secret)),
          tokenLifetimeHours_(tokenLifetimeHours)
    {
        if (secret_.empty())
        {
            throw std::invalid_argument("JWT secret must not be empty");
        }
        if (tokenLifetimeHours_ <= 0)
        {
            throw std::invalid_argument("JWT token lifetime must be positive");
        }
    }

    std::string JwtService::generateToken(
        int userId,
        const std::string &email,
        const std::string &role)
    {
        return jwt::create<JwtTraits>()
            .set_type("JWT")
            .set_issuer(Issuer)
            .set_payload_claim("user_id", jwt::basic_claim<JwtTraits>(std::to_string(userId)))
            .set_payload_claim("email", jwt::basic_claim<JwtTraits>(email))
            .set_payload_claim("role", jwt::basic_claim<JwtTraits>(role))
            .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(tokenLifetimeHours_))
            .sign(jwt::algorithm::hs256{secret_});
    }

    UserClaims JwtService::validateToken(
        const std::string &token)
    {
        const auto decoded = jwt::decode<JwtTraits>(token);

        const auto verifier = jwt::verify<JwtTraits>()
                                  .allow_algorithm(jwt::algorithm::hs256{secret_})
                                  .with_issuer(Issuer);

        verifier.verify(decoded);

        UserClaims claims;
        claims.userId = std::stoi(decoded.get_payload_claim("user_id").as_string());
        claims.email = decoded.get_payload_claim("email").as_string();
        claims.role = decoded.get_payload_claim("role").as_string();

        return claims;
    }
} // namespace dorm_energy::auth

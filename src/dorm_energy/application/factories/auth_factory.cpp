#include "dorm_energy/application/factories/auth_factory.hpp"

#include "dorm_energy/application/auth/auth_service.hpp"
#include "dorm_energy/infrastructure/auth/jwt_service.hpp"
#include "dorm_energy/infrastructure/auth/bcrypt_password_hasher.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    AuthFactory::AuthFactory(
        const AppConfig &config)
        : config_(config) {}

    std::shared_ptr<dorm_energy::auth::AuthService> AuthFactory::create(
        std::shared_ptr<storage::IUserRepository> repository) const
    {
        auto passwordHasher = std::make_shared<auth::BcryptPasswordHasher>();

        const auto jwtConfig = config_.getJwtConfig();

        auto jwtService = std::make_shared<auth::JwtService>(
            jwtConfig.secret,
            jwtConfig.tokenLifetimeHours);

        return std::make_shared<auth::AuthService>(
            std::move(repository),
            std::move(passwordHasher),
            std::move(jwtService));
    }
} // namespace dorm_energy::application::factories

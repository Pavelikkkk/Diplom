#include "dorm_energy/application/factories/auth_factory.hpp"

#include "dorm_energy/application/auth/auth_service.hpp"
#include "dorm_energy/infrastructure/auth/jwt_service.hpp"
#include "dorm_energy/infrastructure/auth/openssl_password_hasher.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    AuthFactory::AuthFactory(
        const AppConfig &config)
        : config_(config) {}

    std::shared_ptr<::AuthService> AuthFactory::create(
        std::shared_ptr<storage::IMeasurementRepository> repository) const
    {
        auto passwordHasher = std::make_shared<OpenSslPasswordHasher>();

        // TODO: move to environment/config.
        auto jwtService = std::make_shared<JwtService>("super-secret-key");

        return std::make_shared<::AuthService>(
            std::move(repository),
            std::move(passwordHasher),
            std::move(jwtService));
    }
} // namespace dorm_energy::application::factories

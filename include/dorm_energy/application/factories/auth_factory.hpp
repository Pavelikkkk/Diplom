#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::auth
{
    class AuthService;
}

namespace dorm_energy::storage
{
    class IUserRepository;
}

namespace dorm_energy::application::factories
{
    class AuthFactory
    {
    public:
        explicit AuthFactory(
            const AppConfig &config);

        std::shared_ptr<dorm_energy::auth::AuthService> create(
            std::shared_ptr<storage::IUserRepository> repository) const;

    private:
        const AppConfig &config_;
    };
} // namespace dorm_energy::application::factories

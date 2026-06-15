#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

class AuthService;

namespace dorm_energy::storage
{
    class IMeasurementRepository;
}

namespace dorm_energy::application::factories
{
    class AuthFactory
    {
    public:
        explicit AuthFactory(const AppConfig &config);

        std::shared_ptr<AuthService>
        create(std::shared_ptr<storage::IMeasurementRepository> repository) const;

    private:
        const AppConfig &config_;
    };
} // namespace dorm_energy::application::factories

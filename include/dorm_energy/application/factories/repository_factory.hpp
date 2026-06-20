#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::storage
{
    class IMeasurementRepository;
} // namespace dorm_energy::storage

namespace dorm_energy::application::factories
{
    class RepositoryFactory
    {
    public:
        explicit RepositoryFactory(
            const AppConfig &config);

        std::shared_ptr<storage::IMeasurementRepository> create();

    private:
        const AppConfig &config_;
        std::shared_ptr<storage::IMeasurementRepository> repository_{};
    };
} // namespace dorm_energy::application::factories

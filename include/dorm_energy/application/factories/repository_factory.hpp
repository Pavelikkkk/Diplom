#pragma once

#include "dorm_energy/application/config/app_config.hpp"
#include "dorm_energy/domain/storage/repositories/iadmin_repository.hpp"
#include "dorm_energy/domain/storage/repositories/ianomaly_repository.hpp"
#include "dorm_energy/domain/storage/repositories/idashboard_repository.hpp"
#include "dorm_energy/domain/storage/repositories/idevice_catalog_repository.hpp"
#include "dorm_energy/domain/storage/repositories/imeasurement_repository.hpp"
#include "dorm_energy/domain/storage/repositories/isubscription_repository.hpp"
#include "dorm_energy/domain/storage/repositories/iuser_repository.hpp"

#include <memory>

namespace dorm_energy::storage
{
    class PostgresMeasurementRepository;
} // namespace dorm_energy::storage

namespace dorm_energy::application::factories
{
    struct RepositoryPorts
    {
        std::shared_ptr<storage::IMeasurementRepository> measurements;
        std::shared_ptr<storage::IAnomalyRepository> anomalies;
        std::shared_ptr<storage::IDashboardRepository> dashboard;
        std::shared_ptr<storage::IDeviceCatalogRepository> catalog;
        std::shared_ptr<storage::IUserRepository> users;
        std::shared_ptr<storage::ISubscriptionRepository> subscriptions;
        std::shared_ptr<storage::IAdminRepository> admin;
    };

    class RepositoryFactory
    {
    public:
        explicit RepositoryFactory(
            const AppConfig &config);

        RepositoryPorts create();

    private:
        std::shared_ptr<storage::PostgresMeasurementRepository> createPostgresRepository();

        const AppConfig &config_;
        std::shared_ptr<storage::PostgresMeasurementRepository> repository_{};
    };
} // namespace dorm_energy::application::factories

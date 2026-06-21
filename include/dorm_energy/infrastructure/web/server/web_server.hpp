#pragma once

#include "dorm_energy/application/auth/auth_service.hpp"
#include "dorm_energy/domain/storage/repositories/iadmin_repository.hpp"
#include "dorm_energy/domain/storage/repositories/ianomaly_repository.hpp"
#include "dorm_energy/domain/storage/repositories/idashboard_repository.hpp"
#include "dorm_energy/domain/storage/repositories/idevice_catalog_repository.hpp"
#include "dorm_energy/domain/storage/repositories/isubscription_repository.hpp"
#include "dorm_energy/domain/storage/repositories/iuser_repository.hpp"
#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"

#include <memory>
#include <thread>

namespace dorm_energy::web
{
    class WebServer
    {
    public:
        WebServer(
            std::shared_ptr<dorm_energy::detection::RoomStateAggregator> aggregator,
            std::shared_ptr<dorm_energy::storage::IAdminRepository> adminRepository,
            std::shared_ptr<dorm_energy::storage::IAnomalyRepository> anomalyRepository,
            std::shared_ptr<dorm_energy::storage::IDashboardRepository> dashboardRepository,
            std::shared_ptr<dorm_energy::storage::IDeviceCatalogRepository> catalogRepository,
            std::shared_ptr<dorm_energy::storage::IUserRepository> userRepository,
            std::shared_ptr<dorm_energy::storage::ISubscriptionRepository> subscriptionRepository,
            std::shared_ptr<dorm_energy::auth::AuthService> authService);

        void start();
        void stop();

    private:
        std::shared_ptr<dorm_energy::detection::RoomStateAggregator> aggregator_;

        std::shared_ptr<dorm_energy::storage::IAdminRepository> adminRepository_;
        std::shared_ptr<dorm_energy::storage::IAnomalyRepository> anomalyRepository_;
        std::shared_ptr<dorm_energy::storage::IDashboardRepository> dashboardRepository_;
        std::shared_ptr<dorm_energy::storage::IDeviceCatalogRepository> catalogRepository_;
        std::shared_ptr<dorm_energy::storage::IUserRepository> userRepository_;
        std::shared_ptr<dorm_energy::storage::ISubscriptionRepository> subscriptionRepository_;

        std::shared_ptr<dorm_energy::auth::AuthService> authService_;

        std::thread serverThread_;
    };
} // namespace dorm_energy::web

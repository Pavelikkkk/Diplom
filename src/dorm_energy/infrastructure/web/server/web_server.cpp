#include "dorm_energy/infrastructure/web/server/web_server.hpp"

#include "dorm_energy/infrastructure/web/context/web_context.hpp"
#include "dorm_energy/infrastructure/web/middleware/cors_middleware.hpp"
#include "dorm_energy/infrastructure/web/routes/route_registrar.hpp"

#include <drogon/drogon.h>

#include <spdlog/spdlog.h>
#include <utility>

namespace dorm_energy::web
{
    WebServer::WebServer(std::shared_ptr<dorm_energy::detection::RoomStateAggregator> aggregator,
                         std::shared_ptr<dorm_energy::storage::IAdminRepository> adminRepository,
                         std::shared_ptr<dorm_energy::storage::IAnomalyRepository> anomalyRepository,
                         std::shared_ptr<dorm_energy::storage::IDashboardRepository> dashboardRepository,
                         std::shared_ptr<dorm_energy::storage::IDeviceCatalogRepository> catalogRepository,
                         std::shared_ptr<dorm_energy::storage::IUserRepository> userRepository,
                         std::shared_ptr<dorm_energy::storage::ISubscriptionRepository> subscriptionRepository,
                         std::shared_ptr<dorm_energy::auth::AuthService> authService)
        : aggregator_(std::move(aggregator)),
          adminRepository_(std::move(adminRepository)),
          anomalyRepository_(std::move(anomalyRepository)),
          dashboardRepository_(std::move(dashboardRepository)),
          catalogRepository_(std::move(catalogRepository)),
          userRepository_(std::move(userRepository)),
          subscriptionRepository_(std::move(subscriptionRepository)),
          authService_(std::move(authService))
    {
    }

    void WebServer::start()
    {
        drogon::app().addListener("0.0.0.0", 8080);

        registerCorsMiddleware();
        registerWebRoutes(WebContext{aggregator_, adminRepository_, anomalyRepository_,
                                     dashboardRepository_, catalogRepository_, userRepository_,
                                     subscriptionRepository_, authService_});

        spdlog::info("Starting Drogon...");

        serverThread_ = std::thread([]() { drogon::app().run(); });
    }

    void WebServer::stop()
    {
        drogon::app().quit();

        if (serverThread_.joinable())
        {
            serverThread_.join();
        }
    }
} // namespace dorm_energy::web

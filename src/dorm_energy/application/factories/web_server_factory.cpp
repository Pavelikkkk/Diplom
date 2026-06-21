#include "dorm_energy/application/factories/web_server_factory.hpp"

#include "dorm_energy/infrastructure/web/server/web_server.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    std::shared_ptr<web::WebServer> WebServerFactory::create(
        std::shared_ptr<detection::RoomStateAggregator> aggregator,
        const RepositoryPorts &repositories,
        std::shared_ptr<dorm_energy::auth::AuthService> authService) const
    {
        return std::make_shared<web::WebServer>(
            std::move(aggregator),
            repositories.admin,
            repositories.anomalies,
            repositories.dashboard,
            repositories.catalog,
            repositories.users,
            repositories.subscriptions,
            std::move(authService));
    }
} // namespace dorm_energy::application::factories

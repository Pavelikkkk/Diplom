#include "dorm_energy/application/factories/web_server_factory.hpp"

#include "dorm_energy/infrastructure/web/server/web_server.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    std::shared_ptr<web::WebServer> WebServerFactory::create(
        std::shared_ptr<detection::RoomStateAggregator> aggregator,
        std::shared_ptr<storage::IMeasurementRepository> repository,
        std::shared_ptr<::AuthService> authService) const
    {
        return std::make_shared<web::WebServer>(
            std::move(aggregator),
            std::move(repository),
            std::move(authService));
    }
} // namespace dorm_energy::application::factories

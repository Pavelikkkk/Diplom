#pragma once

#include "dorm_energy/application/factories/repository_factory.hpp"

#include <memory>

namespace dorm_energy::auth
{
    class AuthService;
}
namespace dorm_energy::detection
{
    class RoomStateAggregator;
} // namespace dorm_energy::detection

namespace dorm_energy::web
{
    class WebServer;
} // namespace dorm_energy::web

namespace dorm_energy::application::factories
{
    class WebServerFactory
    {
    public:
        std::shared_ptr<web::WebServer> create(
            std::shared_ptr<detection::RoomStateAggregator> aggregator,
            const RepositoryPorts &repositories,
            std::shared_ptr<dorm_energy::auth::AuthService> authService) const;
    };
} // namespace dorm_energy::application::factories

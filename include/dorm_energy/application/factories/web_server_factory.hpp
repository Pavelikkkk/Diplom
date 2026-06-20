#pragma once

#include <memory>

class AuthService;

namespace dorm_energy::detection
{
    class RoomStateAggregator;
} // namespace dorm_energy::detection

namespace dorm_energy::storage
{
    class IMeasurementRepository;
} // namespace dorm_energy::storage

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
            std::shared_ptr<storage::IMeasurementRepository> repository,
            std::shared_ptr<::AuthService> authService) const;
    };
} // namespace dorm_energy::application::factories

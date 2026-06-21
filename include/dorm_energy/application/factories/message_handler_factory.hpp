#pragma once

#include "dorm_energy/application/factories/repository_factory.hpp"

#include <memory>

namespace dorm_energy::application
{
    class IMessageHandler;
} // namespace dorm_energy::application

namespace dorm_energy::notification
{
    class INotifier;
} // namespace dorm_energy::notification

namespace dorm_energy::detection
{
    class IStateDetector;
    class RoomStateAggregator;
} // namespace dorm_energy::detection

namespace dorm_energy::application::factories
{
    class MessageHandlerFactory
    {
    public:
        std::unique_ptr<IMessageHandler> create(
            std::unique_ptr<detection::IStateDetector> detector,
            const RepositoryPorts &repositories,
            std::unique_ptr<notification::INotifier> notifier,
            std::shared_ptr<detection::RoomStateAggregator> aggregator) const;
    };
} // namespace dorm_energy::application::factories

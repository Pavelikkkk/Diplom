#include "dorm_energy/application/factories/message_handler_factory.hpp"

#include "dorm_energy/infrastructure/handlers/message_handler.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    std::unique_ptr<IMessageHandler> MessageHandlerFactory::create(
        std::unique_ptr<detection::IStateDetector> detector,
        const RepositoryPorts &repositories,
        std::unique_ptr<notification::INotifier> notifier,
        std::shared_ptr<detection::RoomStateAggregator> aggregator) const
    {
        return std::make_unique<handlers::MessageHandler>(
            std::move(detector),
            repositories.measurements,
            repositories.anomalies,
            repositories.catalog,
            std::move(notifier),
            std::move(aggregator));
    }
} // namespace dorm_energy::application::factories

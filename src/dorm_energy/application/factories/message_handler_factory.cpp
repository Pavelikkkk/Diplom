#include "dorm_energy/application/factories/message_handler_factory.hpp"

#include "dorm_energy/infrastructure/handlers/message_handler.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    std::unique_ptr<IMessageHandler> MessageHandlerFactory::create(
        std::unique_ptr<detection::IStateDetector> detector,
        std::shared_ptr<storage::IMeasurementRepository> repository,
        std::unique_ptr<INotifier> notifier,
        std::shared_ptr<detection::RoomStateAggregator> aggregator) const
    {
        return std::make_unique<handlers::MessageHandler>(
            std::move(detector),
            std::move(repository),
            std::move(notifier),
            std::move(aggregator));
    }
} // namespace dorm_energy::application::factories

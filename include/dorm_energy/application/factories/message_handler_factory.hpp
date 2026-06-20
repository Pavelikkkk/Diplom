#pragma once

#include <memory>

namespace dorm_energy::application
{
    class IMessageHandler;
    class INotifier;
} // namespace dorm_energy::application

namespace dorm_energy::detection
{
    class IStateDetector;
    class RoomStateAggregator;
} // namespace dorm_energy::detection

namespace dorm_energy::storage
{
    class IMeasurementRepository;
} // namespace dorm_energy::storage

namespace dorm_energy::application::factories
{
    class MessageHandlerFactory
    {
    public:
        std::unique_ptr<IMessageHandler> create(
            std::unique_ptr<detection::IStateDetector> detector,
            std::shared_ptr<storage::IMeasurementRepository> repository,
            std::unique_ptr<INotifier> notifier,
            std::shared_ptr<detection::RoomStateAggregator> aggregator) const;
    };
} // namespace dorm_energy::application::factories

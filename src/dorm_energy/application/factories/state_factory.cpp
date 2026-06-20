#include "dorm_energy/application/factories/state_factory.hpp"

#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    StateFactory::StateFactory(
        const AppConfig &config)
        : config_(config)
    {
    }

    std::shared_ptr<detection::RoomStateAggregator> StateFactory::createAggregator()
    {
        if (!aggregator_)
        {
            aggregator_ = std::make_shared<detection::RoomStateAggregator>(
                config_.getRoomStateAggregatorConfig());
        }

        return aggregator_;
    }
} // namespace dorm_energy::application::factories

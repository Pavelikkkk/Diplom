#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::detection
{
    class RoomStateAggregator;
} // namespace dorm_energy::detection 

namespace dorm_energy::application::factories
{
    class StateFactory
    {
    public:
        explicit StateFactory(
            const AppConfig &config);

        std::shared_ptr<detection::RoomStateAggregator> createAggregator();

    private:
        const AppConfig &config_;
        std::shared_ptr<detection::RoomStateAggregator> aggregator_{};
    };
} // namespace dorm_energy::application::factories

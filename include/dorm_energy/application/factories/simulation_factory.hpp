#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::simulation
{
    class IDataGenerator;
} // namespace dorm_energy::simulation

namespace dorm_energy::application::factories
{
    class SimulationFactory
    {
    public:
        explicit SimulationFactory(
            const AppConfig &config);

        std::unique_ptr<simulation::IDataGenerator> createGenerator() const;

    private:
        const AppConfig &config_;
    };
} // namespace dorm_energy::application::factories

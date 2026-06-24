#include "dorm_energy/application/factories/simulation_factory.hpp"

#include "dorm_energy/infrastructure/simulation/synthetic_data_generator.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    SimulationFactory::SimulationFactory(const AppConfig &config)
        : config_(config)
    {
    }

    std::unique_ptr<simulation::IDataGenerator> SimulationFactory::createGenerator() const
    {
        return std::make_unique<simulation::SyntheticDataGenerator>(config_.getGeneratorConfig());
    }
} // namespace dorm_energy::application::factories

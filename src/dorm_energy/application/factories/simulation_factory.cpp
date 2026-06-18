#include "dorm_energy/application/factories/simulation_factory.hpp"

#include "dorm_energy/infrastructure/simulation/synthetic_data_generator.hpp"

#include <memory>
#include <stdexcept>
#include <utility>

namespace dorm_energy::application::factories
{
    SimulationFactory::SimulationFactory(const AppConfig &config)
        : config_(config)
    {
    }

    std::unique_ptr<simulation::IDataGenerator> SimulationFactory::createGenerator(
        std::shared_ptr<storage::IMeasurementRepository> repository) const
    {
        if (!repository)
        {
            throw std::invalid_argument("SimulationFactory requires a valid repository");
        }

        return std::make_unique<simulation::SyntheticDataGenerator>(
            config_.getRandomSeed(),
            config_.getInjectAnomalies(),
            config_.getAnomalyRate(),
            std::move(repository));
    }
} // namespace dorm_energy::application::factories
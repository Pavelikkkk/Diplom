#pragma once

#include "dorm_energy/domain/simulation/generated_dataset.hpp"
#include "dorm_energy/domain/simulation/simulation_device.hpp"

#include <vector>

namespace dorm_energy::simulation
{
    class IDataGenerator
    {
    public:
        virtual ~IDataGenerator() = default;

        virtual GeneratedDataset generate(
            int days,
            const std::vector<SimulationDevice> &devices) = 0;
    };
} // namespace dorm_energy::simulation

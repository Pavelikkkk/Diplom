#pragma once

#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/simulation/simulation_label.hpp"

#include <vector>

namespace dorm_energy::simulation
{
    struct GeneratedDataset
    {
        core::ReadingsBatch readings;
        std::vector<SimulationLabel> labels;
    };
} // namespace dorm_energy::simulation

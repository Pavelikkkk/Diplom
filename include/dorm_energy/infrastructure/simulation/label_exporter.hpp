#pragma once

#include "dorm_energy/domain/simulation/simulation_label.hpp"

#include <filesystem>
#include <vector>

namespace dorm_energy::simulation
{
    class LabelExporter
    {
    public:
        static bool exportLabels(
            const std::vector<SimulationLabel> &labels,
            const std::filesystem::path &filePath);
    };
} // namespace dorm_energy::simulation

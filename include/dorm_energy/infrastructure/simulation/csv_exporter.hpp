#pragma once

#include "dorm_energy/core/measurement.hpp"

#include <filesystem>

namespace dorm_energy::simulation
{

    class CsvExporter
    {
    public:
        static bool exportReadings(
            const core::ReadingsBatch &readings,
            const std::filesystem::path &filePath);
    };

} // namespace dorm_energy::simulation
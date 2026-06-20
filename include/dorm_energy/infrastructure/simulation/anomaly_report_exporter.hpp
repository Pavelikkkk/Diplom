#pragma once

#include "dorm_energy/infrastructure/simulation/anomaly_report_row.hpp"

#include <filesystem>
#include <vector>

namespace dorm_energy::simulation
{
    class AnomalyReportExporter
    {
    public:
        static bool exportReport(
            const std::vector<AnomalyReportRow> &rows,
            const std::filesystem::path &filePath);
    };
} // namespace dorm_energy::simulation

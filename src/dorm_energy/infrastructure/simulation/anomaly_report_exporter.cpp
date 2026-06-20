#include "dorm_energy/infrastructure/simulation/anomaly_report_exporter.hpp"

#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/csv_utils.hpp"

#include <fstream>

namespace dorm_energy::simulation
{
    bool AnomalyReportExporter::exportReport(
        const std::vector<AnomalyReportRow> &rows,
        const std::filesystem::path &filePath)
    {
        csv::ensureParentDirectory(filePath);

        std::ofstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        file << "timestamp,device_id,power,motion,light,type,severity,score,description\n";

        for (const auto &row : rows)
        {
            file << csv::formatTimestamp(row.state.timestamp) << ","
                 << csv::escape(row.state.deviceId) << ","
                 << row.state.power << ","
                 << (row.state.motion ? "true" : "false") << ","
                 << row.state.light << ","
                 << csv::escape(row.info.anomalyType) << ","
                 << core::toString(row.info.severity) << ","
                 << row.info.score << ","
                 << csv::escape(row.info.description) << "\n";
        }

        return true;
    }
} // namespace dorm_energy::simulation

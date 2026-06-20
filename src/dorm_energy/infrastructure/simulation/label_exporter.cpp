#include "dorm_energy/infrastructure/simulation/label_exporter.hpp"

#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/csv_utils.hpp"

#include <fstream>

namespace dorm_energy::simulation
{
    bool LabelExporter::exportLabels(
        const std::vector<SimulationLabel> &labels,
        const std::filesystem::path &filePath)
    {
        csv::ensureParentDirectory(filePath);

        std::ofstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        file << "timestamp,device_id,is_anomaly,type,severity,description\n";

        for (const auto &label : labels)
        {
            file << csv::formatTimestamp(label.timestamp) << ","
                 << csv::escape(label.deviceId) << ","
                 << (label.isAnomaly ? "true" : "false") << ","
                 << csv::escape(label.anomalyType) << ","
                 << core::toString(label.severity) << ","
                 << csv::escape(label.description) << "\n";
        }

        return true;
    }
} // namespace dorm_energy::simulation

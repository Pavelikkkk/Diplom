#include "dorm_energy/infrastructure/simulation/csv_exporter.hpp"

#include "dorm_energy/core/csv_utils.hpp"

#include <fstream>

namespace dorm_energy::simulation
{

    bool CsvExporter::exportReadings(
        const core::ReadingsBatch &readings,
        const std::filesystem::path &filePath)
    {
        csv::ensureParentDirectory(filePath);

        std::ofstream file(filePath);

        if (!file.is_open())
            return false;

        file << "timestamp,device_id,sensor_type,value\n";

        for (const auto &reading : readings)
        {
            double value = reading.value;

            if (reading.sensorType == "motion")
            {
                value = reading.boolValue.value_or(false);
            }

            file << csv::formatTimestamp(reading.timestamp) << ","
                 << csv::escape(reading.deviceId) << ","
                 << csv::escape(reading.sensorType) << ","
                 << value << "\n";
        }

        return true;
    }

}

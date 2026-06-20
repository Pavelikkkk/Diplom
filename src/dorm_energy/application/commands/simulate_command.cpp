#include "dorm_energy/application/commands/simulate_command.hpp"

#include "dorm_energy/domain/detection/detection_context.hpp"
#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/simulation/simulation_device.hpp"
#include "dorm_energy/infrastructure/detection/anomaly_tracker.hpp"
#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"
#include "dorm_energy/infrastructure/simulation/anomaly_report_exporter.hpp"
#include "dorm_energy/infrastructure/simulation/csv_exporter.hpp"
#include "dorm_energy/infrastructure/simulation/label_exporter.hpp"
#include "dorm_energy/core/string_utils.hpp"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
    dorm_energy::simulation::SimulationDevice createSimulationDevice(
        const std::string &id)
    {
        dorm_energy::simulation::SimulationDevice device;
        device.deviceId = id;
        return device;
    }

    std::vector<dorm_energy::simulation::SimulationDevice> createSimulationDevices(
        int count)
    {
        std::vector<dorm_energy::simulation::SimulationDevice> devices;
        devices.reserve(count);

        for (int i = 1; i <= count; ++i)
        {
            devices.push_back(createSimulationDevice("sim-room-" + std::to_string(i)));
        }

        return devices;
    }

}

namespace dorm_energy::application
{
    SimulateCommand::SimulateCommand(
        std::shared_ptr<logging::ILogger> logger,
        const AppConfig &config,
        std::unique_ptr<simulation::IDataGenerator> generator,
        std::unique_ptr<detection::IStateDetector> detector)
        : logger_(std::move(logger)),
          config_(config),
          generator_(std::move(generator)),
          detector_(std::move(detector))
    {
        if (!logger_ || !generator_ || !detector_)
        {
            throw std::invalid_argument("SimulateCommand: all dependencies must be provided");
        }
    }

    bool SimulateCommand::canHandle(
        const cli::ParsedCommand &options) const
    {
        return options.type == cli::CommandType::Simulate;
    }

    int SimulateCommand::execute()
    {
        logger_->info("Starting offline simulation dataset generation for " + std::to_string(config_.getSimulationDays()) + " days");

        if (config_.getInjectAnomalies())
        {
            logger_->info("Synthetic anomaly injection enabled");
        }

        const auto devices = createSimulationDevices(1);

        logger_->info("Preparing " + std::to_string(devices.size()) + " simulation device(s)");
        logger_->info("Using simulation device: " + devices.front().deviceId);
        logger_->info("Generating synthetic readings...");

        simulation::GeneratedDataset dataset = generator_->generate(
            config_.getSimulationDays(),
            devices);

        logger_->info("Generated " + std::to_string(dataset.readings.size()) + " sensor readings and " + std::to_string(dataset.labels.size()) + " labels");

        logger_->info("Exporting generated readings...");

        if (!simulation::CsvExporter::exportReadings(dataset.readings, config_.getSimulationDatasetPath()))
        {
            logger_->error("Failed to export readings dataset to " + config_.getSimulationDatasetPath());
            return 1;
        }

        logger_->info("Exported readings dataset to " + config_.getSimulationDatasetPath());

        logger_->info("Exporting generated labels...");

        if (!simulation::LabelExporter::exportLabels(dataset.labels, config_.getSimulationLabelsPath()))
        {
            logger_->error("Failed to export labels to " + config_.getSimulationLabelsPath());
            return 1;
        }

        logger_->info("Exported labels to " + config_.getSimulationLabelsPath());

        detection::RoomStateAggregator aggregator; 
        detection::AnomalyTracker tracker;

        int anomalyCount{0};
        int ruleCount{0};
        int mlCount{0};
        std::vector<simulation::AnomalyReportRow> reportRows;

        logger_->info("Running detector pipeline over generated readings...");

        for (const auto &reading : dataset.readings)
        {
            auto state = aggregator.update(reading);

            if (!state.has_value())
            {
                logger_->debug("Skipping detector pass until room state is complete for " + reading.deviceId);
                continue;
            }

            detection::DetectionContext context;
            context.current = *state;
            context.history = &aggregator.getHistory(state->deviceId);

            auto info = detector_->detect(context);

            aggregator.commitState(*state);

            if (!info.isAnomaly)
            {
                continue;
            }

            if (!tracker.shouldReport(*state, info))
            {
                continue;
            }

            ++anomalyCount;
            reportRows.push_back({*state, info});

            if (core::startsWith(info.anomalyType, "rule_"))
            {
                ++ruleCount;
            }
            else if (core::startsWith(info.anomalyType, "ml_"))
            {
                ++mlCount;
            }

            logger_->warn("[ANOMALY] " + state->deviceId + " -> " + info.anomalyType + " score=" + std::to_string(info.score));
        }

        logger_->info("Detected anomalies: " + std::to_string(anomalyCount));
        logger_->info("Rule anomalies: " + std::to_string(ruleCount));
        logger_->info("ML anomalies: " + std::to_string(mlCount));

        if (simulation::AnomalyReportExporter::exportReport(reportRows, config_.getSimulationAnomalyReportPath()))
        {
            logger_->info("Exported anomaly report to " + config_.getSimulationAnomalyReportPath());
        }
        else
        {
            logger_->error("Failed to export anomaly report to " + config_.getSimulationAnomalyReportPath());
            return 1;
        }

        logger_->info("Offline simulation completed successfully");

        return 0;
    }
} // namespace dorm_energy::application

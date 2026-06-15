#include "dorm_energy/application/commands/simulate_command.hpp"

#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/detection/istate_detector.hpp"
#include "dorm_energy/domain/simulation/idata_generator.hpp"
#include "dorm_energy/domain/storage/imeasurement_repository.hpp"
#include "dorm_energy/domain/logging/ilogger.hpp"
#include "dorm_energy/core/detection_context.hpp"
#include "dorm_energy/infrastructure/detection/anomaly_tracker.hpp"

#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"
#include "dorm_energy/infrastructure/simulation/csv_exporter.hpp"

#include <iostream>

namespace dorm_energy::application
{

    SimulateCommand::SimulateCommand(
        std::shared_ptr<dorm_energy::logging::ILogger> logger,
        const AppConfig &config,
        std::unique_ptr<dorm_energy::simulation::IDataGenerator> generator,
        std::unique_ptr<dorm_energy::detection::IStateDetector> detector,
        std::shared_ptr<dorm_energy::storage::IMeasurementRepository> repository)
        : logger_(std::move(logger)),
          config_(config),
          generator_(std::move(generator)),
          detector_(std::move(detector)),
          repository_(std::move(repository))
    {
        if (!logger_ || !generator_ || !detector_ || !repository_)
        {
            throw std::invalid_argument("SimulateCommand: all dependencies must be provided");
        }
    }

    bool SimulateCommand::canHandle(const cli::CommandOptions &options) const
    {
        return options.type == cli::CommandType::Simulate;
    }

    int SimulateCommand::execute(
        const cli::CommandOptions &)
    {
        logger_->info("Starting simulation for " + std::to_string(config_.getSimulationDays()) + " days");

        if (config_.getInjectAnomalies())
        {
            logger_->info("Anomaly injection mode enabled");
        }

        core::ReadingsBatch batch = generator_->generate_for_days(config_.getSimulationDays());

        logger_->info("Generated " + std::to_string(batch.size()) + " sensor readings");

        simulation::CsvExporter::exportReadings(batch, "../../data/training_dataset.csv");

        detection::RoomStateAggregator aggregator;
        detection::AnomalyTracker tracker;

        int anomalyCount{0};
        int savedAnomalies{0};
        int ruleCount{0};
        int mlCount{0};

        for (const auto &reading : batch)
        {
            auto state = aggregator.update(reading);

            if (!state.has_value())
                continue;

            detection::DetectionContext context;

            context.current = *state;
            context.history = &aggregator.getHistory(state->deviceId);

            auto info = detector_->detect(context);

            if (!info.isAnomaly)
                continue;

            if (!tracker.shouldReport(*state, info))
                continue;

            ++anomalyCount;

            if (info.anomalyType.starts_with("rule_"))
                ++ruleCount;

            else if (info.anomalyType == "ml_anomaly")
                ++mlCount;
                
            bool success =
                repository_->saveAnomaly(
                    reading,
                    info.anomalyType,
                    info.severity,
                    info.description,
                    info.score);

            if (success)
                ++savedAnomalies;

            std::cout << "[ANOMALY] " << state->deviceId << " -> " << info.anomalyType << " score=" << info.score << '\n';
        }

        logger_->info("Detected anomalies: " + std::to_string(anomalyCount));

        logger_->info("Rule anomalies: " + std::to_string(ruleCount));

        logger_->info("ML anomalies: " + std::to_string(mlCount));

        logger_->info("Saved anomalies: " + std::to_string(savedAnomalies));

        std::size_t saved = repository_->saveBatch(batch);

        logger_->info("Saved " + std::to_string(saved) + " readings to the database");

        logger_->info("Simulation completed successfully");

        return 0;
    }

} // namespace dorm_energy::application

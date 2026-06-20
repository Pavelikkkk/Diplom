#pragma once

#include "dorm_energy/application/config/generator_config.hpp"
#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/aliases.hpp"
#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/simulation/generated_dataset.hpp"
#include "dorm_energy/domain/simulation/idata_generator.hpp"
#include "dorm_energy/domain/simulation/simulation_device.hpp"
#include "dorm_energy/infrastructure/simulation/synthetic_data_types.hpp"

#include <chrono>
#include <random>
#include <string>
#include <vector>

namespace dorm_energy::simulation
{

    class SyntheticDataGenerator : public IDataGenerator
    {
    public:
        explicit SyntheticDataGenerator(
            SyntheticDataGeneratorConfig config = {});

        SyntheticDataGenerator(
            int seed,
            bool injectAnomalies,
            double anomalyRate);

        GeneratedDataset generate(
            int days,
            const std::vector<SimulationDevice> &devices) override;

    private:
        enum class TimeOfDay
        {
            Night,
            Morning,
            Day,
            Evening
        };

        SyntheticSample generateSyntheticSample(
            const std::string &deviceId,
            core::TimePoint timestamp);

        TimeOfDay getTimeOfDay(
            core::TimePoint timestamp) const;

        double getMotionProbability(
            TimeOfDay timeOfDay) const;

        bool generateMotion(
            double probability);

        void generateNormalValues(
            SyntheticSample &sample);

        void applyTimeOfDayAdjustment(
            SyntheticSample &sample,
            TimeOfDay timeOfDay) const;

        std::vector<SyntheticScenario> createScenarios(
            int days,
            const std::vector<SimulationDevice> &devices,
            core::TimePoint start);

        const SyntheticScenario *findScenario(
            const std::vector<SyntheticScenario> &scenarios,
            const std::string &deviceId,
            core::TimePoint timestamp) const;

        void applyScenarioEffect(
            SyntheticSample &sample,
            const SyntheticScenario &scenario);

        void applyScenarioLabelIfMature(
            SyntheticSample &sample,
            const SyntheticScenario &scenario) const;

        void applyPointAnomalyIfNeeded(
            SyntheticSample &sample);

        core::SensorReading makeMotionReading(
            const SyntheticSample &sample) const;

        core::SensorReading makePowerReading(
            const SyntheticSample &sample) const;

        core::SensorReading makeLightReading(
            const SyntheticSample &sample) const;

        SimulationLabel makeLabel(
            const SyntheticSample &sample) const;

        SyntheticDataGeneratorConfig config_;

        std::mt19937 rng_;
    };
}

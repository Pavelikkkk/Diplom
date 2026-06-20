#include "dorm_energy/infrastructure/simulation/synthetic_data_generator.hpp"

#include "dorm_energy/core/time_utils.hpp"

#include <chrono>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace dorm_energy::simulation
{
    namespace
    {
        constexpr int HoursPerDay = 24;
        constexpr int MinutesPerHour = 60;

        double randomDouble(std::mt19937 &rng, double min, double max)
        {
            return std::uniform_real_distribution<double>(min, max)(rng);
        }

        int randomInt(std::mt19937 &rng, int min, int max)
        {
            return std::uniform_int_distribution<int>(min, max)(rng);
        }

        int samplesPerHour(int minutesBetweenSamples)
        {
            return MinutesPerHour / minutesBetweenSamples;
        }

        std::chrono::minutes elapsedMinutes(core::TimePoint from, core::TimePoint to)
        {
            return std::chrono::duration_cast<std::chrono::minutes>(to - from);
        }
    }

    SyntheticDataGenerator::SyntheticDataGenerator(
        SyntheticDataGeneratorConfig config)
        : config_(config),
          rng_(static_cast<std::mt19937::result_type>(config_.seed))
    {
        if (config_.minutesBetweenSamples <= 0)
        {
            throw std::invalid_argument("minutesBetweenSamples must be greater than zero");
        }

        if (MinutesPerHour % config_.minutesBetweenSamples != 0)
        {
            throw std::invalid_argument("minutesBetweenSamples must divide 60 evenly");
        }

        if (config_.pointAnomalyRate < 0.0 ||
            config_.pointAnomalyRate > 1.0)
        {
            throw std::invalid_argument("pointAnomalyRate must be between 0.0 and 1.0");
        }

        if (config_.scenarioAnomalyRate < 0.0 ||
            config_.scenarioAnomalyRate > 1.0)
        {
            throw std::invalid_argument("scenarioAnomalyRate must be between 0.0 and 1.0");
        }
    }

    SyntheticDataGenerator::SyntheticDataGenerator(
        int seed,
        bool injectAnomalies,
        double anomalyRate)
        : SyntheticDataGenerator(
              SyntheticDataGeneratorConfig{
                  .seed = seed,
                  .injectAnomalies = injectAnomalies,
                  .pointAnomalyRate = anomalyRate,
                  .scenarioAnomalyRate = anomalyRate})
    {
    }

    GeneratedDataset SyntheticDataGenerator::generate(
        int days,
        const std::vector<SimulationDevice> &devices)
    {
        if (days <= 0)
        {
            throw std::invalid_argument(
                "days must be greater than zero");
        }

        GeneratedDataset dataset;

        if (devices.empty())
        {
            return dataset;
        }

        const auto now = std::chrono::system_clock::now();
        const auto start = now - std::chrono::hours(HoursPerDay * days);

        const auto scenarios = createScenarios(days, devices, start);

        const int sampleCountPerHour = samplesPerHour(config_.minutesBetweenSamples);

        for (int day = 0; day < days; ++day)
        {
            for (int hour = 0; hour < HoursPerDay; ++hour)
            {
                for (int sample = 0; sample < sampleCountPerHour; ++sample)
                {
                    const auto timestamp = start + std::chrono::hours(day * HoursPerDay + hour) + std::chrono::minutes(sample * config_.minutesBetweenSamples);

                    for (const auto &device : devices)
                    {
                        SyntheticSample sampleState = generateSyntheticSample(device.deviceId, timestamp);

                        if (const auto *scenario = findScenario(scenarios, device.deviceId, timestamp))
                        {
                            applyScenarioEffect(sampleState, *scenario);

                            applyScenarioLabelIfMature(sampleState, *scenario);
                        }
                        else
                        {
                            applyPointAnomalyIfNeeded(sampleState);
                        }

                        dataset.readings.push_back(makeMotionReading(sampleState));

                        dataset.readings.push_back(makePowerReading(sampleState));

                        dataset.readings.push_back(makeLightReading(sampleState));

                        dataset.labels.push_back(makeLabel(sampleState));
                    }
                }
            }
        }

        return dataset;
    }

    SyntheticSample
    SyntheticDataGenerator::generateSyntheticSample(
        const std::string &deviceId,
        core::TimePoint timestamp)
    {
        const TimeOfDay timeOfDay = getTimeOfDay(timestamp);

        const double motionProbability = getMotionProbability(timeOfDay);

        SyntheticSample sample;
        sample.deviceId = deviceId;
        sample.timestamp = timestamp;
        sample.motion = generateMotion(motionProbability);

        generateNormalValues(sample);
        applyTimeOfDayAdjustment(sample, timeOfDay);

        return sample;
    }

    SyntheticDataGenerator::TimeOfDay
    SyntheticDataGenerator::getTimeOfDay(
        core::TimePoint timestamp) const
    {
        const int hour =
            core::extractLocalHour(timestamp);

        if (hour < 6)
        {
            return TimeOfDay::Night;
        }

        if (hour < 10)
        {
            return TimeOfDay::Morning;
        }

        if (hour < 18)
        {
            return TimeOfDay::Day;
        }

        return TimeOfDay::Evening;
    }

    double SyntheticDataGenerator::getMotionProbability(
        TimeOfDay timeOfDay) const
    {
        switch (timeOfDay)
        {
        case TimeOfDay::Night:
            return config_.nightMotionProbability;

        case TimeOfDay::Morning:
            return config_.morningMotionProbability;

        case TimeOfDay::Day:
            return config_.dayMotionProbability;

        case TimeOfDay::Evening:
            return config_.eveningMotionProbability;
        }

        return config_.eveningMotionProbability;
    }

    bool SyntheticDataGenerator::generateMotion(
        double probability)
    {
        std::bernoulli_distribution distribution(probability);
        return distribution(rng_);
    }

    void SyntheticDataGenerator::generateNormalValues(
        SyntheticSample &sample)
    {
        if (sample.motion)
        {
            sample.power = randomDouble(rng_, config_.activePowerMin, config_.activePowerMax);

            sample.light = randomDouble(rng_, config_.activeLightMin, config_.activeLightMax);
        }
        else
        {
            sample.power = randomDouble(rng_, config_.idlePowerMin, config_.idlePowerMax);

            sample.light = randomDouble(rng_, config_.idleLightMin, config_.idleLightMax);
        }
    }

    void SyntheticDataGenerator::applyTimeOfDayAdjustment(
        SyntheticSample &sample,
        TimeOfDay timeOfDay) const
    {
        switch (timeOfDay)
        {
        case TimeOfDay::Night:
            sample.power *= config_.nightPowerMultiplier;

            if (!sample.motion)
            {
                sample.light *= config_.nightIdleLightMultiplier;
            }

            break;

        case TimeOfDay::Morning:
            sample.power *= config_.morningPowerMultiplier;
            break;

        case TimeOfDay::Day:
            sample.power *= config_.dayPowerMultiplier;
            break;

        case TimeOfDay::Evening:
            sample.power *= config_.eveningPowerMultiplier;
            break;
        }
    }

    std::vector<SyntheticScenario>
    SyntheticDataGenerator::createScenarios(
        int days,
        const std::vector<SimulationDevice> &devices,
        core::TimePoint start)
    {
        std::vector<SyntheticScenario> scenarios;

        if (!config_.injectAnomalies || devices.empty())
        {
            return scenarios;
        }

        for (const auto &device : devices)
        {
            for (int day = 0; day < days; ++day)
            {
                if (randomDouble(rng_, 0.0, 1.0) > config_.scenarioAnomalyRate)
                {
                    continue;
                }

                const int startHour = randomInt(rng_, 8, 20);

                const int startMinute = randomInt(rng_, 0, samplesPerHour(config_.minutesBetweenSamples) - 1) * config_.minutesBetweenSamples;

                SyntheticScenario scenario;
                scenario.deviceId = device.deviceId;
                scenario.start = start + std::chrono::hours(day * HoursPerDay + startHour) + std::chrono::minutes(startMinute);

                const int scenarioType = randomInt(rng_, 0, 3);

                switch (scenarioType)
                {
                case 0:
                    scenario.type = ScenarioType::SuddenPowerSpike;
                    scenario.end = scenario.start + std::chrono::minutes(config_.minutesBetweenSamples);
                    break;

                case 1:
                    scenario.type = ScenarioType::RepeatedPowerSpikes;
                    scenario.end = scenario.start + config_.scenarioShortDuration;
                    break;

                case 2:
                    scenario.type = ScenarioType::SustainedHighPower;
                    scenario.end = scenario.start + config_.scenarioLongDuration;
                    break;

                case 3:
                    scenario.type = ScenarioType::UnattendedPowerUsage;
                    scenario.end = scenario.start + config_.scenarioLongDuration;
                    break;

                default:
                    scenario.type = ScenarioType::None;
                    scenario.end = scenario.start;
                    break;
                }

                if (scenario.type != ScenarioType::None)
                {
                    scenarios.push_back(scenario);
                }
            }
        }

        return scenarios;
    }

    const SyntheticScenario *
    SyntheticDataGenerator::findScenario(
        const std::vector<SyntheticScenario> &scenarios,
        const std::string &deviceId,
        core::TimePoint timestamp) const
    {
        for (const auto &scenario : scenarios)
        {
            if (scenario.deviceId != deviceId)
            {
                continue;
            }

            if (timestamp >= scenario.start &&
                timestamp <= scenario.end)
            {
                return &scenario;
            }
        }

        return nullptr;
    }

    void SyntheticDataGenerator::applyScenarioEffect(
        SyntheticSample &sample,
        const SyntheticScenario &scenario)
    {
        switch (scenario.type)
        {
        case ScenarioType::SuddenPowerSpike:
            sample.power += randomDouble(rng_, config_.suddenSpikePowerMin, config_.suddenSpikePowerMax);
            break;

        case ScenarioType::RepeatedPowerSpikes:
        {
            const auto elapsed = elapsedMinutes(scenario.start, sample.timestamp).count();

            const bool highPhase = (elapsed / config_.minutesBetweenSamples) % 2 == 0;

            if (highPhase)
            {
                sample.power = randomDouble(rng_, config_.repeatedSpikeHighPowerMin, config_.repeatedSpikeHighPowerMax);
            }
            else
            {
                sample.power = randomDouble(rng_, config_.repeatedSpikeLowPowerMin, config_.repeatedSpikeLowPowerMax);
            }

            break;
        }

        case ScenarioType::SustainedHighPower:
            sample.power = randomDouble(rng_, config_.sustainedHighPowerMin, config_.sustainedHighPowerMax);
            break;

        case ScenarioType::UnattendedPowerUsage:
            sample.motion = false;
            sample.power = randomDouble(rng_, config_.unattendedPowerMin, config_.unattendedPowerMax);
            break;

        case ScenarioType::None:
            break;
        }
    }

    void SyntheticDataGenerator::applyScenarioLabelIfMature(
        SyntheticSample &sample,
        const SyntheticScenario &scenario) const
    {
        const auto elapsed = elapsedMinutes(scenario.start, sample.timestamp);

        switch (scenario.type)
        {
        case ScenarioType::SuddenPowerSpike:
            sample.isAnomaly = true;
            sample.anomalyType = "rule_sudden_power_spike";
            sample.description = "Injected sudden power spike";
            break;

        case ScenarioType::RepeatedPowerSpikes:
        {
            const int spikeCount = static_cast<int>(elapsed.count() / (config_.minutesBetweenSamples * 2));

            if (elapsed >= config_.repeatedSpikeWindow &&
                spikeCount >= config_.repeatedSpikeMinCount)
            {
                sample.isAnomaly = true;
                sample.anomalyType = "rule_repeated_power_spikes";
                sample.description = "Injected repeated power spikes";
            }

            break;
        }

        case ScenarioType::SustainedHighPower:
            if (elapsed >= config_.sustainedHighPowerWindow)
            {
                sample.isAnomaly = true;
                sample.anomalyType = "rule_sustained_high_power";
                sample.description = "Injected sustained high power usage";
            }

            break;

        case ScenarioType::UnattendedPowerUsage:
            if (elapsed >= config_.unattendedWindow)
            {
                sample.isAnomaly = true;
                sample.anomalyType = "rule_unattended_power_usage";
                sample.description = "Injected unattended power usage";
            }

            break;

        case ScenarioType::None:
            break;
        }
    }

    void SyntheticDataGenerator::applyPointAnomalyIfNeeded(
        SyntheticSample &sample)
    {
        const bool shouldInjectAnomaly = config_.injectAnomalies && randomDouble(rng_, 0.0, 1.0) < config_.pointAnomalyRate;

        if (!shouldInjectAnomaly)
        {
            return;
        }

        const int anomalyType = randomInt(rng_, 0, 3);

        sample.isAnomaly = true;

        switch (anomalyType)
        {
        case 0:
            sample.power = randomDouble(rng_, config_.negativePowerMin, config_.negativePowerMax);
            sample.anomalyType = "rule_negative_power";
            sample.description = "Injected negative power value";
            break;

        case 1:
            sample.light = randomDouble(rng_, config_.negativeLightMin, config_.negativeLightMax);
            sample.anomalyType = "rule_negative_light";
            sample.description = "Injected negative light value";
            break;

        case 2:
            sample.power = randomDouble(rng_, config_.extremePowerMin, config_.extremePowerMax);
            sample.anomalyType = "rule_extreme_power";
            sample.description = "Injected extreme room power value";
            break;

        case 3:
            sample.light = randomDouble(rng_, config_.extremeLightMin, config_.extremeLightMax);
            sample.anomalyType = "rule_extreme_light";
            sample.description = "Injected extreme indoor light value";
            break;

        default:
            sample.isAnomaly = false;
            sample.anomalyType.clear();
            sample.description.clear();
            break;
        }
    }

    core::SensorReading SyntheticDataGenerator::makeMotionReading(
        const SyntheticSample &sample) const
    {
        core::SensorReading reading;
        reading.timestamp = sample.timestamp;
        reading.deviceId = sample.deviceId;
        reading.sensorType = "motion";
        reading.boolValue = sample.motion;
        reading.unit = "bool";

        return reading;
    }

    core::SensorReading SyntheticDataGenerator::makePowerReading(
        const SyntheticSample &sample) const
    {
        core::SensorReading reading;
        reading.timestamp = sample.timestamp;
        reading.deviceId = sample.deviceId;
        reading.sensorType = "power";
        reading.value = sample.power;
        reading.unit = "kW";

        return reading;
    }

    core::SensorReading SyntheticDataGenerator::makeLightReading(
        const SyntheticSample &sample) const
    {
        core::SensorReading reading;
        reading.timestamp = sample.timestamp;
        reading.deviceId = sample.deviceId;
        reading.sensorType = "light";
        reading.value = sample.light;
        reading.unit = "lux";

        return reading;
    }

    SimulationLabel SyntheticDataGenerator::makeLabel(
        const SyntheticSample &sample) const
    {
        SimulationLabel label;
        label.timestamp = sample.timestamp;
        label.deviceId = sample.deviceId;
        label.isAnomaly = sample.isAnomaly;
        label.anomalyType = sample.anomalyType;
        label.description = sample.description;
        label.severity =
            sample.isAnomaly
                ? core::AlertSeverity::Warning
                : core::AlertSeverity::Info;

        return label;
    }
} // namespace dorm_energy::simulation

#include "dorm_energy/infrastructure/detection/rule_based_detector.hpp"

#include <chrono>
#include <cmath>
#include <deque>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace dorm_energy::detection
{
    namespace
    {
        void validateConfig(
            const RuleBasedDetectorConfig &config)
        {
            if (config.extremePowerKw <= 0.0)
            {
                throw std::invalid_argument("extremePowerKw must be greater than zero");
            }

            if (config.extremeLightLux <= 0.0)
            {
                throw std::invalid_argument("extremeLightLux must be greater than zero");
            }

            if (config.sustainedHighPowerKw <= 0.0)
            {
                throw std::invalid_argument("sustainedHighPowerKw must be greater than zero");
            }

            if (config.sustainedHighPowerWindow <= std::chrono::minutes{0})
            {
                throw std::invalid_argument("sustainedHighPowerWindow must be greater than zero");
            }

            if (config.unattendedPowerKw <= 0.0)
            {
                throw std::invalid_argument("unattendedPowerKw must be greater than zero");
            }

            if (config.unattendedWindow <= std::chrono::minutes{0})
            {
                throw std::invalid_argument("unattendedWindow must be greater than zero");
            }

            if (config.suddenPowerSpikeKw <= 0.0)
            {
                throw std::invalid_argument("suddenPowerSpikeKw must be greater than zero");
            }

            if (config.repeatedSpikeDeltaKw <= 0.0)
            {
                throw std::invalid_argument("repeatedSpikeDeltaKw must be greater than zero");
            }

            if (config.repeatedSpikeMinCount <= 0)
            {
                throw std::invalid_argument("repeatedSpikeMinCount must be greater than zero");
            }

            if (config.repeatedSpikeWindow <= std::chrono::minutes{0})
            {
                throw std::invalid_argument("repeatedSpikeWindow must be greater than zero");
            }
        }

        AnomalyInfo makeNormal()
        {
            return {};
        }

        AnomalyInfo makeAnomaly(
            std::string anomalyType,
            std::string description,
            core::AlertSeverity severity,
            float score = 1.0f)
        {
            AnomalyInfo info;
            info.isAnomaly = true;
            info.anomalyType = std::move(anomalyType);
            info.description = std::move(description);
            info.severity = severity;
            info.score = score;

            return info;
        }

        bool hasInvalidNumbers(
            const core::RoomState &state)
        {
            return !std::isfinite(state.power) || !std::isfinite(state.light);
        }

        std::vector<core::RoomState> statesInWindow(
            const std::deque<core::RoomState> &history,
            const core::RoomState &current,
            std::chrono::minutes window)
        {
            std::vector<core::RoomState> result;
            const auto from = current.timestamp - window;

            for (const auto &state : history)
            {
                if (state.timestamp >= from && state.timestamp <= current.timestamp)
                {
                    result.push_back(state);
                }
            }

            result.push_back(current);
            return result;
        }

        bool coversWindow(
            const std::vector<core::RoomState> &states,
            std::chrono::minutes window)
        {
            if (states.size() < 2)
            {
                return false;
            }

            const auto duration = states.back().timestamp - states.front().timestamp;
            return duration >= window;
        }

        bool hasSustainedHighPower(
            const std::deque<core::RoomState> &history,
            const core::RoomState &current,
            double thresholdKw,
            std::chrono::minutes window)
        {
            const auto states = statesInWindow(history, current, window);

            if (!coversWindow(states, window))
            {
                return false;
            }

            for (const auto &state : states)
            {
                if (state.power < thresholdKw)
                {
                    return false;
                }
            }

            return true;
        }

        bool hasUnattendedPowerUsage(
            const std::deque<core::RoomState> &history,
            const core::RoomState &current,
            double thresholdKw,
            std::chrono::minutes window)
        {
            const auto states = statesInWindow(history, current, window);

            if (!coversWindow(states, window))
            {
                return false;
            }

            for (const auto &state : states)
            {
                if (state.motion || state.power < thresholdKw)
                {
                    return false;
                }
            }

            return true;
        }

        bool hasSuddenPowerSpike(
            const std::deque<core::RoomState> &history,
            const core::RoomState &current,
            double spikeDeltaKw)
        {
            if (history.empty())
            {
                return false;
            }

            const auto &previous = history.back();
            return current.power - previous.power >= spikeDeltaKw;
        }

        bool hasRepeatedPowerSpikes(
            const std::deque<core::RoomState> &history,
            const core::RoomState &current,
            double spikeDeltaKw,
            int minSpikeCount,
            std::chrono::minutes window)
        {
            const auto states = statesInWindow(history, current, window);

            if (states.size() < 2)
            {
                return false;
            }

            int spikeCount = 0;

            for (std::size_t i = 1; i < states.size(); ++i)
            {
                const double delta = states[i].power - states[i - 1].power;

                if (delta >= spikeDeltaKw)
                {
                    ++spikeCount;
                }
            }

            return spikeCount >= minSpikeCount;
        }
    } // namespace

    RuleBasedDetector::RuleBasedDetector(
        RuleBasedDetectorConfig config)
        : config_(config)
    {
        validateConfig(config_);
    }

    AnomalyInfo RuleBasedDetector::detect(
        const DetectionContext &context) const
    {
        const auto &state = context.current;
        const auto *history = context.history;

        if (hasInvalidNumbers(state))
        {
            return makeAnomaly(
                "rule_invalid_number",
                "Room state contains a non-finite numeric value",
                core::AlertSeverity::Warning);
        }

        if (state.power < 0.0)
        {
            return makeAnomaly(
                "rule_negative_power",
                "Power value is negative",
                core::AlertSeverity::Warning);
        }

        if (state.light < 0.0)
        {
            return makeAnomaly(
                "rule_negative_light",
                "Light value is negative",
                core::AlertSeverity::Warning);
        }

        if (state.power > config_.extremePowerKw)
        {
            return makeAnomaly(
                "rule_extreme_power",
                "Power value is above configured room maximum",
                core::AlertSeverity::Warning);
        }

        if (state.light > config_.extremeLightLux)
        {
            return makeAnomaly(
                "rule_extreme_light",
                "Light value is above configured indoor maximum",
                core::AlertSeverity::Info);
        }

        if (history && hasUnattendedPowerUsage(*history, state, config_.unattendedPowerKw, config_.unattendedWindow))
        {
            return makeAnomaly(
                "rule_unattended_power_usage",
                "Power stayed high while no motion was detected for a long period",
                core::AlertSeverity::Warning);
        }

        if (history && hasSustainedHighPower(*history, state, config_.sustainedHighPowerKw, config_.sustainedHighPowerWindow))
        {
            return makeAnomaly(
                "rule_sustained_high_power",
                "Power consumption stayed high for too long",
                core::AlertSeverity::Warning);
        }

        if (history && hasRepeatedPowerSpikes(*history, state, config_.repeatedSpikeDeltaKw, config_.repeatedSpikeMinCount, config_.repeatedSpikeWindow))
        {
            return makeAnomaly(
                "rule_repeated_power_spikes",
                "Repeated power spikes detected within the configured window",
                core::AlertSeverity::Info);
        }

        if (history && hasSuddenPowerSpike(*history, state, config_.suddenPowerSpikeKw))
        {
            return makeAnomaly(
                "rule_sudden_power_spike",
                "Sudden power consumption spike detected",
                core::AlertSeverity::Info);
        }

        return makeNormal();
    }
} // namespace dorm_energy::detection

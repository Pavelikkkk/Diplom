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

            if (config.baselineMinPowerSamples <= 0)
            {
                throw std::invalid_argument("baselineMinPowerSamples must be greater than zero");
            }

            if (config.baselineSpikeMarginKw <= 0.0)
            {
                throw std::invalid_argument("baselineSpikeMarginKw must be greater than zero");
            }

            if (config.baselineSustainedMarginKw <= 0.0)
            {
                throw std::invalid_argument("baselineSustainedMarginKw must be greater than zero");
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

        AnomalyInfo makeNormal() { return {}; }

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

            // Проверяем каждое состояние внутри окна.
            // Если хотя бы в одном состоянии мощность ниже порога,
            // значит высокая мощность не была постоянной.
            for (const auto &state : states)
            {
                if (state.power < thresholdKw)
                {
                    return false;
                }
            }

            // Если все состояния в окне имеют мощность выше порога,
            // значит есть длительное высокое энергопотребление.
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
                // Если в какой-то момент было движение
                // или мощность упала ниже порога,
                // значит это не unattended power usage.
                if (state.motion || state.power < thresholdKw)
                {
                    return false;
                }
            }

            // Если всё окно не было движения,
            // но мощность оставалась выше порога,
            // считаем это потреблением без присутствия человека.
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

            // Проверяем резкий рост мощности.
            // Аномалия есть, если текущая мощность выросла относительно предыдущей
            // минимум на spikeDeltaKw.
            return current.power - previous.power >= spikeDeltaKw;
        }

        bool hasBaselinePowerSpike(
            const DetectionContext &context,
            const RuleBasedDetectorConfig &config)
        {
            if (!context.baselineAveragePowerKw.has_value())
            {
                return false;
            }

            if (context.baselinePowerSampleCount < config.baselineMinPowerSamples)
            {
                return false;
            }

            // Проверяем, насколько текущая мощность выше обычного среднего уровня.
            // Аномалия есть, если:
            // current power >= baseline average + допустимый margin.
            return context.current.power >= *context.baselineAveragePowerKw + config.baselineSpikeMarginKw;
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

            // Счётчик резких скачков мощности.
            int spikeCount = 0;

            for (std::size_t i = 1; i < states.size(); ++i)
            {
                // Разница между текущей и предыдущей мощностью.
                const double delta = states[i].power - states[i - 1].power;

                // Если рост мощности больше или равен порогу,
                // считаем это одним скачком.
                if (delta >= spikeDeltaKw)
                {
                    ++spikeCount;
                }
            }

            // Аномалия есть, если количество скачков в окне
            // достигло минимального требуемого количества.
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

        // Проверяем, готов ли baseline для использования.
        // Baseline готов, если средняя базовая мощность уже рассчитана
        // и количество измерений достаточно для доверия к этому значению.
        const bool baselineReady =
            context.baselineAveragePowerKw.has_value() && context.baselinePowerSampleCount >= config_.baselineMinPowerSamples;

        // Рассчитываем порог высокой мощности на основе baseline.
        // Если baseline готов, берём среднюю базовую мощность и добавляем допустимый запас.
        // Если baseline не готов, используем стандартный порог sustainedHighPowerKw из config_.
        const double baselineSustainedPowerKw =
            baselineReady ? *context.baselineAveragePowerKw + config_.baselineSustainedMarginKw : config_.sustainedHighPowerKw;

        // Определяем итоговый порог для правила длительного высокого потребления.
        // Если baseline готов, используем порог на основе baseline.
        // Если baseline не готов, берём maxNormalPowerKw из профиля комнаты,
        // а если его нет — используем дефолтный sustainedHighPowerKw из config_.
        const double sustainedHighPowerKw =
            baselineReady ? baselineSustainedPowerKw : context.maxNormalPowerKw.value_or(config_.sustainedHighPowerKw);

        // Определяем итоговый порог для правила потребления без присутствия.
        // Если baseline готов, используем тот же baseline-based порог.
        // Если baseline не готов, берём maxNormalPowerKw из профиля комнаты,
        // а если его нет — используем дефолтный unattendedPowerKw из config_.
        const double unattendedPowerKw =
            baselineReady ? baselineSustainedPowerKw : context.maxNormalPowerKw.value_or(config_.unattendedPowerKw);

        // Проверяем правило потребления энергии без присутствия человека.
        // Условие срабатывает только если есть история состояний,
        // в профиле комнаты запрещено unattended-потребление,
        // и в течение заданного окна мощность была высокой,
        // пока движение не фиксировалось.        
        if (history && !context.allowUnattendedPower &&
            hasUnattendedPowerUsage(*history, state, unattendedPowerKw, config_.unattendedWindow))
        {
            return makeAnomaly(
                "rule_unattended_power_usage",
                "Power stayed high while no motion was detected for a long period",
                core::AlertSeverity::Critical);
        }

        // Проверяем правило длительного высокого энергопотребления.
        // Условие срабатывает, если есть история состояний
        // и мощность оставалась выше порога sustainedHighPowerKw
        // в течение configured sustainedHighPowerWindow.
        if (history &&
            hasSustainedHighPower(*history, state, sustainedHighPowerKw, config_.sustainedHighPowerWindow))
        {
            return makeAnomaly(
                "rule_sustained_high_power",
                "Power consumption stayed high for too long",
                core::AlertSeverity::Warning);
        }

        // Проверяем скачок мощности относительно baseline.
        // Это правило сравнивает текущую мощность
        // со средней базовой мощностью комнаты/устройства.
        // Если текущее значение выше baseline + margin,
        // создаётся информационная аномалия.
        if (hasBaselinePowerSpike(context, config_))
        {
            return makeAnomaly(
                "rule_baseline_power_spike",
                "Power consumption is above the learned room baseline",
                core::AlertSeverity::Info);
        }

        // Проверяем повторяющиеся скачки мощности.
        // Условие срабатывает, если в заданном временном окне
        // количество резких ростов мощности достигло repeatedSpikeMinCount.
        if (history && hasRepeatedPowerSpikes(*history, state, config_.repeatedSpikeDeltaKw, config_.repeatedSpikeMinCount, config_.repeatedSpikeWindow))
        {
            return makeAnomaly(
                "rule_repeated_power_spikes",
                "Repeated power spikes detected within the configured window",
                core::AlertSeverity::Info);
        }

        // Проверяем внезапный скачок мощности.
        // Сравнивается текущее значение мощности с предыдущим состоянием.
        // Если рост больше или равен suddenPowerSpikeKw,
        // создаётся информационная аномалия.
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

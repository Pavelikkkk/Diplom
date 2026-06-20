#pragma once

#include "dorm_energy/core/aliases.hpp"

#include <string>

namespace dorm_energy::simulation
{
    enum class ScenarioType
    {
        None,
        SuddenPowerSpike,
        RepeatedPowerSpikes,
        SustainedHighPower,
        UnattendedPowerUsage
    };

    struct SyntheticScenario
    {
        std::string deviceId{};

        core::TimePoint start{};
        core::TimePoint end{};

        ScenarioType type{ScenarioType::None};
    };

    struct SyntheticSample
    {
        bool motion{false};

        double power{0.0};
        double light{0.0};

        std::string deviceId{};
        core::TimePoint timestamp{};

        bool isAnomaly{false};

        std::string anomalyType{};
        std::string description{};
    };
}

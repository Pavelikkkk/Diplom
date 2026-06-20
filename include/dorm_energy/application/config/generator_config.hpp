#pragma once

#include <chrono>

namespace dorm_energy::simulation
{
    struct SyntheticDataGeneratorConfig
    {
        int seed{42};

        bool injectAnomalies{true};

        double pointAnomalyRate{0.01};

        double scenarioAnomalyRate{0.05};

        int minutesBetweenSamples{10};

        double nightMotionProbability{0.02};
        double morningMotionProbability{0.75};
        double dayMotionProbability{0.45};
        double eveningMotionProbability{0.65};

        double activePowerMin{0.3};
        double activePowerMax{1.5};

        double idlePowerMin{0.05};
        double idlePowerMax{0.3};

        double activeLightMin{300.0};
        double activeLightMax{800.0};

        double idleLightMin{0.0};
        double idleLightMax{30.0};

        double nightPowerMultiplier{0.7};
        double nightIdleLightMultiplier{0.3};
        double morningPowerMultiplier{1.1};
        double dayPowerMultiplier{1.0};
        double eveningPowerMultiplier{1.2};

        double negativePowerMin{-2.0};
        double negativePowerMax{-0.1};

        double negativeLightMin{-500.0};
        double negativeLightMax{-1.0};

        double extremePowerMin{5.5};
        double extremePowerMax{8.0};

        double extremeLightMin{21000.0};
        double extremeLightMax{30000.0};

        double suddenSpikePowerMin{3.0};
        double suddenSpikePowerMax{4.5};

        double repeatedSpikeLowPowerMin{0.2};
        double repeatedSpikeLowPowerMax{0.8};

        double repeatedSpikeHighPowerMin{3.0};
        double repeatedSpikeHighPowerMax{5.0};

        int repeatedSpikeMinCount{4};
        std::chrono::minutes repeatedSpikeWindow{60};

        double sustainedHighPowerMin{2.8};
        double sustainedHighPowerMax{4.2};
        std::chrono::minutes sustainedHighPowerWindow{120};

        double unattendedPowerMin{1.8};
        double unattendedPowerMax{3.2};
        std::chrono::minutes unattendedWindow{120};

        std::chrono::minutes scenarioShortDuration{60};
        std::chrono::minutes scenarioLongDuration{150};
    };
}
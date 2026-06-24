#pragma once

#include <chrono>

namespace dorm_energy::detection
{
    struct RuleBasedDetectorConfig
    {
        double extremePowerKw{25.0};
        double extremeLightLux{20000.0};

        double sustainedHighPowerKw{2.8};
        std::chrono::minutes sustainedHighPowerWindow{120};

        double unattendedPowerKw{1.8};
        std::chrono::minutes unattendedWindow{120};

        double suddenPowerSpikeKw{3.0};
        int baselineMinPowerSamples{20};
        double baselineSpikeMarginKw{1.5};
        double baselineSustainedMarginKw{0.8};

        double repeatedSpikeDeltaKw{2.0};
        int repeatedSpikeMinCount{4};
        std::chrono::minutes repeatedSpikeWindow{60};
    };
} // namespace dorm_energy::detection

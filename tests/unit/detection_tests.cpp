#include "dorm_energy/infrastructure/detection/anomaly_tracker.hpp"
#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"
#include "dorm_energy/infrastructure/detection/rule_based_detector.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <deque>
#include <stdexcept>

using dorm_energy::core::AlertSeverity;
using dorm_energy::core::RoomState;
using dorm_energy::core::SensorReading;
using dorm_energy::detection::AnomalyInfo;
using dorm_energy::detection::DetectionContext;
using dorm_energy::detection::RuleBasedDetector;

TEST(RuleBasedDetectorTest, DetectsExtremePower)
{
    dorm_energy::detection::RuleBasedDetectorConfig config;
    config.extremePowerKw = 5.0;
    RuleBasedDetector detector(config);

    DetectionContext context{};
    context.current.power = 5.01;

    const auto result = detector.detect(context);

    EXPECT_TRUE(result.isAnomaly);
    EXPECT_EQ(result.anomalyType, "rule_extreme_power");
    EXPECT_EQ(result.severity, AlertSeverity::Warning);
}

TEST(RuleBasedDetectorTest, DetectsExtremeLight)
{
    dorm_energy::detection::RuleBasedDetectorConfig config;
    config.extremeLightLux = 500.0;
    RuleBasedDetector detector(config);

    DetectionContext context{};
    context.current.light = 501.0;

    const auto result = detector.detect(context);

    EXPECT_TRUE(result.isAnomaly);
    EXPECT_EQ(result.anomalyType, "rule_extreme_light");
    EXPECT_EQ(result.severity, AlertSeverity::Info);
}

TEST(RuleBasedDetectorTest, DetectsUnattendedPowerUsage)
{
    dorm_energy::detection::RuleBasedDetectorConfig config;
    config.unattendedPowerKw = 1.0;
    config.unattendedWindow = std::chrono::minutes(30);
    RuleBasedDetector detector(config);

    const auto start = std::chrono::system_clock::from_time_t(1717243200);

    std::deque<RoomState> history{
        RoomState{.deviceId = "room-101", .timestamp = start, .motion = false, .power = 1.5},
    };

    DetectionContext context{};
    context.current = RoomState{.deviceId = "room-101", .timestamp = start + std::chrono::minutes(30), .motion = false, .power = 1.5};
    context.history = &history;

    const auto result = detector.detect(context);

    EXPECT_TRUE(result.isAnomaly);
    EXPECT_EQ(result.anomalyType, "rule_unattended_power_usage");
}

TEST(RuleBasedDetectorTest, IgnoresNormalStateAndRejectsInvalidThreshold)
{
    dorm_energy::detection::RuleBasedDetectorConfig config;
    RuleBasedDetector detector(config);

    DetectionContext context{};
    context.current.power = 6.0;
    context.current.motion = true;
    context.current.light = 250.0;

    EXPECT_FALSE(detector.detect(context).isAnomaly);

    config.extremePowerKw = -1.0;
    EXPECT_THROW(RuleBasedDetector invalidDetector(config), std::invalid_argument);
}

TEST(RoomStateAggregatorTest, AggregatesReadingsAndKeepsThirtyMinuteHistory)
{
    dorm_energy::detection::RoomStateAggregator aggregator;
    const auto start = std::chrono::system_clock::from_time_t(1717243200);

    aggregator.update(SensorReading{.timestamp = start, .deviceId = "room-101", .sensorType = "motion", .value = 0.0, .boolValue = true});
    aggregator.update(SensorReading{.timestamp = start + std::chrono::minutes(10), .deviceId = "room-101", .sensorType = "power", .value = 2.25});
    const auto current = aggregator.update(SensorReading{.timestamp = start + std::chrono::minutes(31), .deviceId = "room-101", .sensorType = "light", .value = 700.0});

    ASSERT_TRUE(current.has_value());
    EXPECT_EQ(current->deviceId, "room-101");
    EXPECT_TRUE(current->motion);
    EXPECT_DOUBLE_EQ(current->power, 2.25);
    EXPECT_DOUBLE_EQ(current->light, 700.0);

    aggregator.commitState(*current);

    EXPECT_EQ(aggregator.getCurrentStates().size(), 1U);
    EXPECT_EQ(aggregator.getHistory("room-101").size(), 1U);
    EXPECT_TRUE(aggregator.getHistory("missing").empty());
}

TEST(AnomalyTrackerTest, SuppressesDuplicatesUntilRoomIsResolved)
{
    dorm_energy::detection::AnomalyTracker tracker;
    RoomState state{};
    state.deviceId = "room-101";

    AnomalyInfo anomaly{};
    anomaly.anomalyType = "rule_high_power";

    EXPECT_TRUE(tracker.shouldReport(state, anomaly));
    EXPECT_FALSE(tracker.shouldReport(state, anomaly));

    tracker.resolveRoom("room-101");

    EXPECT_TRUE(tracker.shouldReport(state, anomaly));
}

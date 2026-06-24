#include "dorm_energy/infrastructure/mqtt/message_parser.hpp"

#include <gtest/gtest.h>

using dorm_energy::mqtt::MessageParser;

TEST(MessageParserTest, RejectsPayloadsThatCannotBeParsed)
{
    const MessageParser parser;

    EXPECT_FALSE(parser.canParse(""));
    EXPECT_FALSE(parser.canParse("plain text"));
    EXPECT_FALSE(parser.canParse(std::string(4097, '{')));
    EXPECT_FALSE(parser.parse("").has_value());
    EXPECT_FALSE(parser.parse("{not json").has_value());
}

TEST(MessageParserTest, ParsesCamelCaseNumericPayload)
{
    const MessageParser parser;

    const auto reading = parser.parse(
        R"({"deviceId":"room-101","sensorType":"power","value":2.5,"unit":"kW"})");

    ASSERT_TRUE(reading.has_value());
    EXPECT_EQ(reading->deviceId, "room-101");
    EXPECT_EQ(reading->sensorType, "power");
    EXPECT_DOUBLE_EQ(reading->value, 2.5);
    EXPECT_EQ(reading->unit, "kW");
    EXPECT_FALSE(reading->boolValue.has_value());
}

TEST(MessageParserTest, ParsesSnakeCaseBooleanPayload)
{
    const MessageParser parser;

    const auto reading = parser.parse(
        R"({"device_id":"room-102","sensor_type":"motion","bool_value":true})");

    ASSERT_TRUE(reading.has_value());
    EXPECT_EQ(reading->deviceId, "room-102");
    EXPECT_EQ(reading->sensorType, "motion");
    EXPECT_TRUE(reading->boolValue.value());
}

TEST(MessageParserTest, TreatsNumericMotionValueAsBoolean)
{
    const MessageParser parser;

    const auto reading = parser.parse(
        R"({"deviceId":"room101","sensorType":"motion","value":1,"unit":"bool"})");

    ASSERT_TRUE(reading.has_value());
    EXPECT_EQ(reading->deviceId, "room101");
    EXPECT_EQ(reading->sensorType, "motion");
    ASSERT_TRUE(reading->boolValue.has_value());
    EXPECT_TRUE(reading->boolValue.value());
    EXPECT_DOUBLE_EQ(reading->value, 1.0);
    EXPECT_EQ(reading->unit, "bool");
}

TEST(MessageParserTest, ConvertsPowerWattsToKilowatts)
{
    const MessageParser parser;

    const auto reading = parser.parse(
        R"({"deviceId":"room101","sensorType":"power","value":702.5,"unit":"W"})");

    ASSERT_TRUE(reading.has_value());
    EXPECT_EQ(reading->deviceId, "room101");
    EXPECT_EQ(reading->sensorType, "power");
    EXPECT_DOUBLE_EQ(reading->value, 0.7025);
    EXPECT_EQ(reading->unit, "kW");
}

TEST(MessageParserTest, UsesDocumentedDefaultsForMissingFields)
{
    const MessageParser parser;

    const auto reading = parser.parse(R"({"numeric_value":7.0})");

    ASSERT_TRUE(reading.has_value());
    EXPECT_EQ(reading->deviceId, "unknown");
    EXPECT_EQ(reading->sensorType, "unknown");
    EXPECT_DOUBLE_EQ(reading->value, 7.0);
    EXPECT_EQ(reading->unit, "");
}

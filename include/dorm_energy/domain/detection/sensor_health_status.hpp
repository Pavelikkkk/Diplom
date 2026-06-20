#pragma once

namespace dorm_energy::detection
{
    enum class SensorHealthStatus
    {
        Ok,

        Incomplete,

        UnknownSensorType,

        InvalidMotionValue,

        InvalidNumericValue
    };
}
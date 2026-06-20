#include "dorm_energy/core/time_utils.hpp"

#include <chrono>
#include <ctime>

namespace dorm_energy::core
{
    int extractLocalHour(TimePoint timestamp)
    {
        const auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm localTime{};

#ifdef _WIN32
        localtime_s(&localTime, &time);
#else
        localtime_r(&time, &localTime);
#endif

        return localTime.tm_hour;
    }
}
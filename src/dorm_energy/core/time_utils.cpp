#include "dorm_energy/core/time_utils.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace dorm_energy::core
{
        namespace
        {
                std::tm toLocalTime(
                    TimePoint timestamp)
                {
                        const auto time = std::chrono::system_clock::to_time_t(timestamp);

                        std::tm localTime{};

#ifdef _WIN32
                        localtime_s(
                            &localTime,
                            &time);
#else
                        localtime_r(
                            &time,
                            &localTime);
#endif

                        return localTime;
                }
        }

        int extractLocalHour(
            TimePoint timestamp)
        {
                return toLocalTime(timestamp).tm_hour;
        }

        std::string formatLocalTimestamp(
            TimePoint timestamp)
        {
                const auto localTime = toLocalTime(timestamp);

                std::ostringstream stream;

                stream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");

                return stream.str();
        }
}
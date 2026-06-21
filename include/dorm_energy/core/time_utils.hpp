#pragma once

#include "dorm_energy/core/aliases.hpp"

namespace dorm_energy::core
{
    int extractLocalHour(TimePoint timestamp);

    std::string formatLocalTimestamp(
        TimePoint timestamp);
}

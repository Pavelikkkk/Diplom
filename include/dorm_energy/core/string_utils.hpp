#pragma once

#include <string>

namespace dorm_energy::core
{
    inline bool startsWith(
        const std::string &value,
        const std::string &prefix)
    {
        return value.rfind(prefix, 0) == 0;
    }
}
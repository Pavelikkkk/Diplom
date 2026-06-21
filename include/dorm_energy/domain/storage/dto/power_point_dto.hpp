#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct PowerPointDto
    {
        std::string time;
        double power{0.0};
    };
} // namespace dorm_energy::storage
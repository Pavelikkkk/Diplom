#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct AnomalyDto
    {
        std::string room;
        std::string type;
        std::string severity;
        double score{0.0};
        std::string description;
        std::string detectedAt;
    };
} // namespace dorm_energy::storage
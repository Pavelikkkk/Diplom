#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct BuildingDto
    {
        int id = 0;

        std::string name;

        std::string address;

        std::string description;
    };
} // namespace dorm_energy::storage
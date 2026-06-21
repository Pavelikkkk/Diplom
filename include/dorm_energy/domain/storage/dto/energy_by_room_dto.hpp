#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct EnergyByRoomDto
    {
        std::string roomName;
        double avgPower{};
    };
} // namespace dorm_energy::storage
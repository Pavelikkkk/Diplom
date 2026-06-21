#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct SubscriptionDto
    {
        std::string plan{"STANDARD"};
        std::string status{"ACTIVE"};
        int maxBuildings{0};
        int maxRooms{5};
        int maxDevices{20};
    };
} // namespace dorm_energy::storage

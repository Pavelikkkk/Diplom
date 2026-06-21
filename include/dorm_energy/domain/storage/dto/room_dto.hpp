#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct RoomDto
    {
        int id{};

        int buildingId{};

        std::string roomName;

        std::string roomType;

        int floorNumber{};
    };
} // namespace dorm_energy::storage
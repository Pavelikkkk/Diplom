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

        double minNormalPowerKw{0.0};

        double maxNormalPowerKw{2.8};

        bool allowUnattendedPower{false};

        bool motion{false};

        double power{0.0};

        double light{0.0};
    };
} // namespace dorm_energy::storage

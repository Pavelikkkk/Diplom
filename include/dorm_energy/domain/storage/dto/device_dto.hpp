#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct DeviceDto
    {
        int roomId{0};

        int buildingId{0};

        int organizationId{0};

        std::string deviceId;

        std::string deviceName;

        std::string deviceModel;

        std::string firmwareVersion;

        std::string roomName;

        bool isOnline = false;

        std::string lastSeenAt;
    };
} // namespace dorm_energy::storage
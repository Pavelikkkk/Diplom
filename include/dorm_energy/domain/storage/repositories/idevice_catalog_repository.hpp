#pragma once

#include "dorm_energy/domain/storage/dto/building_dto.hpp"
#include "dorm_energy/domain/storage/dto/device_dto.hpp"
#include "dorm_energy/domain/storage/dto/room_dto.hpp"

#include <string>
#include <vector>

namespace dorm_energy::storage
{
    class IDeviceCatalogRepository
    {
    public:
        virtual ~IDeviceCatalogRepository() = default;

        virtual std::vector<DeviceDto> getDevices(
            int organizationId = 0) = 0;

        virtual std::vector<BuildingDto> getBuildings(
            int organizationId = 0) = 0;

        virtual std::vector<RoomDto> getRooms(
            int organizationId = 0) = 0;

        virtual int createBuildingForOrganization(
            int organizationId,
            const std::string &name,
            const std::string &address,
            const std::string &description) = 0;

        virtual int createRoomForBuilding(
            int buildingId,
            const std::string &roomName,
            const std::string &roomType,
            int floorNumber) = 0;

        virtual bool createDeviceForRoom(
            const std::string &deviceId,
            const std::string &deviceName,
            const std::string &deviceModel,
            const std::string &firmwareVersion,
            int roomId) = 0;
    };
} // namespace dorm_energy::storage

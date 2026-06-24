#pragma once

#include "dorm_energy/domain/storage/dto/building_dto.hpp"
#include "dorm_energy/domain/storage/dto/device_dto.hpp"
#include "dorm_energy/domain/storage/dto/room_detection_profile_dto.hpp"
#include "dorm_energy/domain/storage/dto/room_dto.hpp"

#include <optional>
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

        virtual std::optional<RoomDetectionProfileDto> getRoomDetectionProfileByDeviceId(
            const std::string &deviceId) = 0;

        virtual int createBuildingForOrganization(
            int organizationId,
            const std::string &name,
            const std::string &address,
            const std::string &description) = 0;

        virtual int createRoomForBuilding(
            int buildingId,
            const std::string &roomName,
            const std::string &roomType,
            int floorNumber,
            double minNormalPowerKw = 0.0,
            double maxNormalPowerKw = 2.8,
            bool allowUnattendedPower = false) = 0;

        virtual bool createDeviceForRoom(
            const std::string &deviceId,
            const std::string &deviceName,
            const std::string &deviceModel,
            const std::string &firmwareVersion,
            int roomId) = 0;

        virtual bool updateBuilding(
            int buildingId,
            int organizationId,
            const std::string &name,
            const std::string &address,
            const std::string &description) = 0;

        virtual bool deleteBuilding(
            int buildingId) = 0;

        virtual bool updateRoom(
            int roomId,
            int buildingId,
            const std::string &roomName,
            const std::string &roomType,
            int floorNumber,
            double minNormalPowerKw,
            double maxNormalPowerKw,
            bool allowUnattendedPower) = 0;

        virtual bool deleteRoom(
            int roomId) = 0;

        virtual bool updateDevice(
            const std::string &deviceId,
            const std::string &deviceName,
            const std::string &deviceModel,
            const std::string &firmwareVersion,
            int roomId) = 0;

        virtual bool deleteDevice(
            const std::string &deviceId) = 0;
    };
} // namespace dorm_energy::storage

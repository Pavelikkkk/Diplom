#pragma once

#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/storage/repositories/iadmin_repository.hpp"
#include "dorm_energy/domain/storage/repositories/ianomaly_repository.hpp"
#include "dorm_energy/domain/storage/repositories/idashboard_repository.hpp"
#include "dorm_energy/domain/storage/repositories/idevice_catalog_repository.hpp"
#include "dorm_energy/domain/storage/repositories/imeasurement_repository.hpp"
#include "dorm_energy/domain/storage/repositories/isubscription_repository.hpp"
#include "dorm_energy/domain/storage/repositories/iuser_repository.hpp"

#include <memory>
#include <mutex>
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <unordered_set>
#include <vector>

namespace dorm_energy::storage
{
    class PostgresMeasurementRepository final
        : public IMeasurementRepository,
          public IAnomalyRepository,
          public IDashboardRepository,
          public IDeviceCatalogRepository,
          public IUserRepository,
          public ISubscriptionRepository,
          public IAdminRepository
    {
    public:
        explicit PostgresMeasurementRepository(
            const std::string &connectionString,
            std::size_t maxBufferSize = 2000);

        ~PostgresMeasurementRepository() override;

        // Measurements
        bool saveReading(
            const core::SensorReading &reading) override;

        std::size_t saveBatch(
            const core::ReadingsBatch &readings) override;

        void flush() override;

        bool saveAnomaly(
            const core::SensorReading &reading,
            const std::string &anomalyType,
            core::AlertSeverity severity,
            const std::string &description,
            double score = 0.0) override;

        std::vector<AnomalyDto> getLatestAnomalies(
            std::size_t limit = 20,
            int organizationId = 0) override;

        std::vector<PowerPointDto> getPowerHistory(
            int hours = 24,
            int organizationId = 0,
            int buildingId = 0) override;

        std::vector<TopConsumerDto> getTopConsumers(
            int limit = 10,
            int organizationId = 0,
            int buildingId = 0) override;

        std::vector<AnomalyStatsDto> getAnomalyStatistics(
            int organizationId = 0,
            int buildingId = 0) override;

        std::vector<EnergyByRoomDto> getEnergyByRoom(
            int organizationId = 0,
            int buildingId = 0) override;

        std::vector<SeverityStatsDto> getSeverityDistribution(
            int organizationId = 0,
            int buildingId = 0) override;

        std::vector<DeviceDto> getDevices(
            int organizationId = 0) override;

        std::vector<BuildingDto> getBuildings(
            int organizationId = 0) override;

        std::vector<RoomDto> getRooms(
            int organizationId = 0) override;

        std::optional<RoomDetectionProfileDto> getRoomDetectionProfileByDeviceId(
            const std::string &deviceId) override;

        int createBuildingForOrganization(
            int organizationId,
            const std::string &name,
            const std::string &address,
            const std::string &description) override;

        int createRoomForBuilding(
            int buildingId,
            const std::string &roomName,
            const std::string &roomType,
            int floorNumber,
            double minNormalPowerKw = 0.0,
            double maxNormalPowerKw = 2.8,
            bool allowUnattendedPower = false) override;

        bool createDeviceForRoom(
            const std::string &deviceId,
            const std::string &deviceName,
            const std::string &deviceModel,
            const std::string &firmwareVersion,
            int roomId) override;

        bool updateBuilding(
            int buildingId,
            int organizationId,
            const std::string &name,
            const std::string &address,
            const std::string &description) override;

        bool deleteBuilding(
            int buildingId) override;

        bool updateRoom(
            int roomId,
            int buildingId,
            const std::string &roomName,
            const std::string &roomType,
            int floorNumber,
            double minNormalPowerKw,
            double maxNormalPowerKw,
            bool allowUnattendedPower) override;

        bool deleteRoom(
            int roomId) override;

        bool updateDevice(
            const std::string &deviceId,
            const std::string &deviceName,
            const std::string &deviceModel,
            const std::string &firmwareVersion,
            int roomId) override;

        bool deleteDevice(
            const std::string &deviceId) override;

        std::optional<UserDto> findUserByEmail(
            const std::string &email) override;

        std::optional<UserDto> findUserById(
            int userId) override;

        int createUser(
            const UserDto &user) override;

        bool updateUserTelegramChatId(
            int userId,
            const std::string &telegramChatId) override;

        SubscriptionDto getUserSubscription(
            int userId) override;

        SubscriptionDto upgradeUserSubscription(
            int userId) override;

        Json::Value getAdminOverview() override;

    private:
        void connect();

        bool tryReconnect(
            int maxAttempts = 3);

        void doFlush(
            const std::vector<core::SensorReading> &readings);

        std::vector<std::string> getUnknownDeviceIds(
            const std::vector<core::SensorReading> &readings);

        void markDevicesKnown(
            const std::vector<std::string> &deviceIds);

        void ensureDeviceExists(
            pqxx::work &txn,
            const std::string &deviceId);

    private:
        std::string connectionString_;

        std::unique_ptr<pqxx::connection> connection_;

        std::vector<core::SensorReading> buffer_;

        const std::size_t maxBufferSize_;

        std::mutex bufferMutex_;

        std::mutex deviceCacheMutex_;

        std::unordered_set<std::string> knownDeviceIds_;
    };
} // namespace dorm_energy::storage

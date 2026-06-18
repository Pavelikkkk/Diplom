// include/dorm_energy/infrastructure/storage/postgres_repository.hpp
#pragma once

#include "dorm_energy/core/alert_severity.hpp"
#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/storage/building_dto.hpp"
#include "dorm_energy/domain/storage/device_dto.hpp"
#include "dorm_energy/domain/storage/energy_by_room_dto.hpp"
#include "dorm_energy/domain/storage/imeasurement_repository.hpp"
#include "dorm_energy/domain/storage/power_point_dto.hpp"
#include "dorm_energy/domain/storage/room_dto.hpp"
#include "dorm_energy/domain/storage/severity_stats_dto.hpp"
#include "dorm_energy/domain/storage/user_dto.hpp"

#include <chrono>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <string>
#include <unordered_set>
#include <vector>

namespace dorm_energy::storage
{
    class PostgresMeasurementRepository : public IMeasurementRepository
    {
    public:
        explicit PostgresMeasurementRepository(const std::string &connectionString,
                                               std::size_t maxBufferSize = 2000);

        ~PostgresMeasurementRepository() override;

        bool save(const core::SensorReading &reading) override;
        std::size_t saveBatch(const core::ReadingsBatch &readings) override;

        bool saveAnomaly(const core::SensorReading &reading, const std::string &anomalyType,
                         core::AlertSeverity severity, const std::string &description,
                         double score) override;

        void flush() override;

        std::vector<AnomalyDto> getLatestAnomalies(std::size_t limit = 20,
                                                   int organizationId = 0) override;

        std::vector<PowerPointDto> getPowerHistory(int hours = 24, int organizationId = 0) override;

        std::vector<DeviceDto> getDevices(int organizationId = 0) override;

        std::vector<BuildingDto> getBuildings(int organizationId = 0) override;

        std::vector<RoomDto> getRooms(int organizationId = 0) override;

        std::vector<TopConsumerDto> getTopConsumers(int limit = 10,
                                                    int organizationId = 0) override;

        std::vector<AnomalyStatsDto> getAnomalyStatistics(int organizationId = 0) override;

        std::vector<EnergyByRoomDto> getEnergyByRoom(int organizationId = 0) override;

        std::vector<SeverityStatsDto> getSeverityDistribution(int organizationId = 0) override;

        std::optional<UserDto> findUserByEmail(const std::string &email) override;
        std::optional<UserDto> findUserById(int id) override;
        int createUser(const UserDto &user) override;

        std::optional<UserDto> getUserById(int userId) override;

        bool updateUserTelegramChatId(int userId, const std::string &telegramChatId) override;

        Json::Value getUserSubscription(int userId) override;

        Json::Value getAdminOverview() override;

        int createBuildingForOrganization(int organizationId, const std::string &name,
                                          const std::string &address,
                                          const std::string &description) override;

        int createRoomForBuilding(int buildingId, const std::string &roomName,
                                  const std::string &roomType, int floorNumber) override;

        bool createDeviceForRoom(const std::string &deviceId, const std::string &deviceName,
                                 const std::string &deviceModel, const std::string &firmwareVersion,
                                 int roomId) override;

    private:
        void connect();
        bool tryReconnect(int maxAttempts = 3);

        void doFlush(const std::vector<core::SensorReading> &readings);
        std::vector<std::string>
        getUnknownDeviceIds(const std::vector<core::SensorReading> &readings);
        void markDevicesKnown(const std::vector<std::string> &deviceIds);
        void ensureDeviceExists(pqxx::work &txn, const std::string &deviceId);

        std::string connectionString_;
        std::unique_ptr<pqxx::connection> connection_;

        std::vector<core::SensorReading> buffer_;
        const std::size_t maxBufferSize_;

        std::mutex bufferMutex_; // thread-safety для будущего MQTT
        std::mutex deviceCacheMutex_;
        std::unordered_set<std::string> knownDeviceIds_;
    };
} // namespace dorm_energy::storage

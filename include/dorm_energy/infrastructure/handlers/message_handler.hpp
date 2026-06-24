#pragma once

#include "dorm_energy/application/imessage_handler.hpp"
#include "dorm_energy/domain/notification/inotifier.hpp"

#include "dorm_energy/core/measurement.hpp"
#include "dorm_energy/domain/detection/istate_detector.hpp"
#include "dorm_energy/domain/storage/repositories/ianomaly_repository.hpp"
#include "dorm_energy/domain/storage/repositories/idevice_catalog_repository.hpp"
#include "dorm_energy/domain/storage/repositories/imeasurement_repository.hpp"
#include "dorm_energy/infrastructure/detection/anomaly_tracker.hpp"
#include "dorm_energy/infrastructure/detection/room_state_aggregator.hpp"

#include <memory>
#include <vector>

namespace dorm_energy::handlers
{
    class MessageHandler final : public application::IMessageHandler
    {
    public:
        explicit MessageHandler(
            std::unique_ptr<detection::IStateDetector> detector,
            std::shared_ptr<storage::IMeasurementRepository> measurementRepository,
            std::shared_ptr<storage::IAnomalyRepository> anomalyRepository,
            std::shared_ptr<storage::IDeviceCatalogRepository> catalogRepository,
            std::unique_ptr<notification::INotifier> notifier,
            std::shared_ptr<detection::RoomStateAggregator> aggregator);

        bool handle(
            const core::SensorReading &reading) override;

        std::size_t handleBatch(
            const std::vector<core::SensorReading> &readings) override;

        void flush() override;

    private:
        void persistCurrentBatch();

        std::unique_ptr<detection::IStateDetector> detector_;

        std::shared_ptr<storage::IMeasurementRepository> measurementRepository_;

        std::shared_ptr<storage::IAnomalyRepository> anomalyRepository_;

        std::shared_ptr<storage::IDeviceCatalogRepository> catalogRepository_;

        std::unique_ptr<notification::INotifier> notifier_;

        std::shared_ptr<detection::RoomStateAggregator> aggregator_;

        detection::AnomalyTracker tracker_;

        core::ReadingsBatch batch_;
    };
} // namespace dorm_energy::handlers

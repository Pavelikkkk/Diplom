#include "dorm_energy/infrastructure/handlers/message_handler.hpp"

#include "dorm_energy/application/notification/notification_message_factory.hpp"
#include "dorm_energy/domain/detection/detection_context.hpp"

#include <stdexcept>
#include <utility>

namespace dorm_energy::handlers
{
    namespace
    {
        constexpr std::size_t BatchThreshold = 1; // вынести
    }

    MessageHandler::MessageHandler(
        std::unique_ptr<detection::IStateDetector> detector,
        std::shared_ptr<storage::IMeasurementRepository> measurementRepository,
        std::shared_ptr<storage::IAnomalyRepository> anomalyRepository,
        std::shared_ptr<storage::IDeviceCatalogRepository> catalogRepository,
        std::unique_ptr<notification::INotifier> notifier,
        std::shared_ptr<detection::RoomStateAggregator> aggregator)
        : detector_(std::move(detector)),
          measurementRepository_(std::move(measurementRepository)),
          anomalyRepository_(std::move(anomalyRepository)),
          catalogRepository_(std::move(catalogRepository)),
          notifier_(std::move(notifier)),
          aggregator_(std::move(aggregator))
    {
        if (!detector_)
        {
            throw std::invalid_argument("MessageHandler: detector must not be null");
        }

        if (!measurementRepository_)
        {
            throw std::invalid_argument("MessageHandler: measurementRepository must not be null");
        }

        if (!anomalyRepository_)
        {
            throw std::invalid_argument("MessageHandler: anomalyRepository must not be null");
        }

        if (!catalogRepository_)
        {
            throw std::invalid_argument("MessageHandler: catalogRepository must not be null");
        }

        if (!notifier_)
        {
            throw std::invalid_argument("MessageHandler: notifier must not be null");
        }

        if (!aggregator_)
        {
            throw std::invalid_argument("MessageHandler: aggregator must not be null");
        }
    }

    bool MessageHandler::handle(
        const core::SensorReading &reading)
    {
        batch_.push_back(reading);

        if (batch_.size() >= BatchThreshold)
        {
            persistCurrentBatch();
        }

        const auto updateResult = aggregator_->update(reading);

        if (updateResult.health != detection::SensorHealthStatus::Ok)
        {
            return true;
        }

        if (!updateResult.state.has_value())
        {
            return true;
        }

        const auto &state = updateResult.state.value();

        detection::DetectionContext context;
        context.current = state;
        const auto &history = aggregator_->getHistory(state.deviceId);
        context.history = &history;

        if (const auto profile = catalogRepository_->getRoomDetectionProfileByDeviceId(state.deviceId))
        {
            // Минимальная нормальная мощность для этой комнаты/устройства.
            // Используется детектором как нижняя граница нормального диапазона потребления.
            context.minNormalPowerKw = profile->minNormalPowerKw;

            // Максимальная нормальная мощность для этой комнаты/устройства.
            // Если текущее потребление выше этого значения, детектор может считать это аномалией.
            context.maxNormalPowerKw = profile->maxNormalPowerKw;

            // Флаг, который указывает, разрешено ли потребление энергии без движения.
            // Если false, высокая мощность при отсутствии движения может считаться аномалией.
            context.allowUnattendedPower = profile->allowUnattendedPower;

            // Среднее базовое энергопотребление устройства или комнаты.
            // Используется для сравнения текущей мощности с обычным уровнем потребления.
            context.baselineAveragePowerKw = profile->baselineAveragePowerKw;

            // Количество измерений, на основе которых был рассчитан baseline.
            // Нужно, чтобы понимать, насколько baseline уже надёжный.
            context.baselinePowerSampleCount = profile->baselinePowerSampleCount;

            // Флаг, который показывает, готов ли baseline к использованию.
            // Если baselineReady == false, правила, зависящие от baseline, лучше не применять.
            context.baselineReady = profile->baselineReady;
        }

        const auto anomalyInfo = detector_->detect(context);

        aggregator_->commitState(state);

        if (!anomalyInfo.isAnomaly)
        {
            return true;
        }

        if (!tracker_.shouldReport(state, anomalyInfo))
        {
            return true;
        }

        anomalyRepository_->saveAnomaly(
            reading,
            anomalyInfo.anomalyType,
            anomalyInfo.severity,
            anomalyInfo.description,
            anomalyInfo.score);

        const auto notification = application::makeAnomalyNotification(
            state,
            anomalyInfo);

        notifier_->send(notification);

        return true;
    }

    std::size_t MessageHandler::handleBatch(
        const std::vector<core::SensorReading> &readings)
    {
        if (readings.empty())
        {
            return 0;
        }

        std::size_t processed = 0;

        for (const auto &reading : readings)
        {
            if (handle(reading))
            {
                ++processed;
            }
        }

        return processed;
    }

    void MessageHandler::flush()
    {
        persistCurrentBatch();
    }

    void MessageHandler::persistCurrentBatch()
    {
        if (batch_.empty())
        {
            return;
        }

        const auto batchSize = batch_.size();
        const bool saved = measurementRepository_->saveBatch(batch_);
        
        if (!saved)
        {
            spdlog::error("Failed to persist measurement batch: size={}", batchSize);

            return;
        }

        spdlog::debug("Measurement batch persisted successfully: size={}", batchSize);
        measurementRepository_->saveBatch(batch_);

        batch_.clear();
    }
} // namespace dorm_energy::handlers

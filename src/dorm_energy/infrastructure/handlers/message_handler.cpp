#include "dorm_energy/infrastructure/handlers/message_handler.hpp"

#include "dorm_energy/application/notification/notification_message_factory.hpp"
#include "dorm_energy/domain/detection/detection_context.hpp"

#include <stdexcept>
#include <utility>

namespace dorm_energy::handlers
{
    namespace
    {
        constexpr std::size_t BatchThreshold = 100;
    }

    MessageHandler::MessageHandler(
        std::unique_ptr<detection::IStateDetector> detector,
        std::shared_ptr<storage::IMeasurementRepository> measurementRepository,
        std::shared_ptr<storage::IAnomalyRepository> anomalyRepository,
        std::unique_ptr<notification::INotifier> notifier,
        std::shared_ptr<detection::RoomStateAggregator> aggregator)
        : detector_(std::move(detector)),
          measurementRepository_(std::move(measurementRepository)),
          anomalyRepository_(std::move(anomalyRepository)),
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

        const auto updateResult =
            aggregator_->update(reading);

        if (updateResult.health != detection::SensorHealthStatus::Ok)
        {
            return true;
        }

        if (!updateResult.state.has_value())
        {
            return true;
        }

        const auto &state =
            updateResult.state.value();

        detection::DetectionContext context;
        context.current = state;
        const auto &history = aggregator_->getHistory(state.deviceId);
        context.history = &history;

        const auto anomalyInfo =
            detector_->detect(context);

        aggregator_->commitState(state);

        if (!anomalyInfo.isAnomaly)
        {
            tracker_.resolveRoom(state.deviceId);

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

        const auto notification =
            application::makeAnomalyNotification(
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

        measurementRepository_->saveBatch(batch_);

        batch_.clear();
    }
} // namespace dorm_energy::handlers

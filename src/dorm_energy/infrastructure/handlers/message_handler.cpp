#include "dorm_energy/infrastructure/handlers/message_handler.hpp"
#include "dorm_energy/core/detection_context.hpp"

#include <fmt/format.h>
#include <iostream>

namespace dorm_energy::handlers
{

    MessageHandler::MessageHandler(
        std::unique_ptr<dorm_energy::detection::IStateDetector> detector,
        std::shared_ptr<dorm_energy::storage::IMeasurementRepository> repository,
        std::unique_ptr<dorm_energy::application::INotifier> notifier,
        std::shared_ptr<dorm_energy::detection::RoomStateAggregator> aggregator)
        : detector_(std::move(detector)), repository_(std::move(repository)),
          notifier_(std::move(notifier)), aggregator_(std::move(aggregator))
    {
        if (!detector_ || !repository_ || !notifier_ || !aggregator_)
        {
            throw std::invalid_argument("MessageHandler: all dependencies must be provided");
        }
    }

    bool MessageHandler::handle(const core::SensorReading &reading)
    {
        batch_.push_back(reading);

        constexpr std::size_t BATCH_THRESHOLD = 100;

        if (batch_.size() >= BATCH_THRESHOLD)
        {
            persistCurrentBatch();
        }

        auto state = aggregator_->update(reading);

        if (!state)
        {
            return true;
        }

        detection::DetectionContext context;

        context.current = *state;

        context.history = &aggregator_->getHistory(state->deviceId);

        auto anomalyInfo = detector_->detect(context);

        if (!anomalyInfo.isAnomaly)
        {
            tracker_.resolveRoom(state->deviceId);

            return true;
        }

        if (!tracker_.shouldReport(*state, anomalyInfo))
        {
            return true;
        }

        repository_->saveAnomaly(reading, anomalyInfo.anomalyType, anomalyInfo.severity,
                                 anomalyInfo.description, anomalyInfo.score);

        notifier_->sendAlert(*state, anomalyInfo);

        return true;
    }

    std::size_t MessageHandler::handleBatch(const std::vector<core::SensorReading> &readings)
    {
        if (readings.empty())
            return 0;

        std::size_t processed = 0;

        for (const auto &reading : readings)
        {
            handle(reading);
            ++processed;
        }

        return processed;
    }

    void MessageHandler::flush() { persistCurrentBatch(); }

    void MessageHandler::persistCurrentBatch()
    {
        if (batch_.empty())
            return;

        std::size_t saved = repository_->saveBatch(batch_);

        std::cout << fmt::format(
            "[MessageHandler] Saved {} readings to repository (batch size: {})\n", saved,
            batch_.size());

        batch_.clear();
    }

} // namespace dorm_energy::handlers
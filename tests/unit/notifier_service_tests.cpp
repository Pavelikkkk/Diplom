#include "dorm_energy/application/notification/notifier_service.hpp"

#include <gtest/gtest.h>

using dorm_energy::application::NotifierService;
using dorm_energy::notification::INotifier;
using dorm_energy::notification::NotificationMessage;

namespace
{
    class FakeNotifier final : public INotifier
    {
    public:
        explicit FakeNotifier(bool sendResult)
            : sendResult_(sendResult)
        {
        }

        bool send(const NotificationMessage &) override
        {
            ++sendCalls;
            return sendResult_;
        }

        std::size_t sendBatch(const std::vector<NotificationMessage> &messages) override
        {
            ++sendBatchCalls;
            return sendResult_ ? messages.size() : 0U;
        }

        int sendCalls{0};
        int sendBatchCalls{0};

    private:
        bool sendResult_;
    };
}

TEST(NotifierServiceTest, EmptyServiceDoesNotSend)
{
    NotifierService service;

    EXPECT_FALSE(service.send(NotificationMessage{}));
    EXPECT_EQ(service.sendBatch({NotificationMessage{}}), 0U);
}

TEST(NotifierServiceTest, IgnoresNullNotifiers)
{
    NotifierService service;
    service.addNotifier(nullptr);

    EXPECT_FALSE(service.send(NotificationMessage{}));
}

TEST(NotifierServiceTest, SendsAlertToAllNotifiersAndReportsAggregateSuccess)
{
    NotifierService service;
    auto success = std::make_unique<FakeNotifier>(true);
    auto failure = std::make_unique<FakeNotifier>(false);
    auto *successPtr = success.get();
    auto *failurePtr = failure.get();

    service.addNotifier(std::move(success));
    service.addNotifier(std::move(failure));

    EXPECT_FALSE(service.send(NotificationMessage{}));
    EXPECT_EQ(successPtr->sendCalls, 1);
    EXPECT_EQ(failurePtr->sendCalls, 1);
}

TEST(NotifierServiceTest, SumsBatchResultsAcrossNotifiers)
{
    NotifierService service;
    service.addNotifier(std::make_unique<FakeNotifier>(true));
    service.addNotifier(std::make_unique<FakeNotifier>(true));

    EXPECT_EQ(service.sendBatch({NotificationMessage{}, NotificationMessage{}}), 4U);
    EXPECT_EQ(service.sendBatch({}), 0U);
}

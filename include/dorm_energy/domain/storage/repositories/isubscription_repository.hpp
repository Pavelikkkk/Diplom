#pragma once

#include "dorm_energy/domain/storage/dto/subscription_dto.hpp"

namespace dorm_energy::storage
{
    class ISubscriptionRepository
    {
    public:
        virtual ~ISubscriptionRepository() = default;

        virtual SubscriptionDto getUserSubscription(
            int userId) = 0;
    };
} // namespace dorm_energy::storage

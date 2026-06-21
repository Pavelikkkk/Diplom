#pragma once

#include <json/json.h>

namespace dorm_energy::storage
{
    class IAdminRepository
    {
    public:
        virtual ~IAdminRepository() = default;

        virtual Json::Value getAdminOverview() = 0;
    };
} // namespace dorm_energy::storage

#pragma once

#include <string>

namespace dorm_energy::storage
{
    struct TopConsumerDto
    {
        std::string roomName;

        double power{};
    };
} // namespace dorm_energy::storage
#pragma once

#include <string>

namespace dorm_energy::application
{
    struct MqttConfig
    {
        std::string broker{};
        std::string clientId{"dorm-energy"};
        std::string topic{"devices/+/+"};
        std::string username{};
        std::string password{};
        bool tlsVerify{false};
    };
} // namespace dorm_energy::application

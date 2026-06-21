#pragma once

#include <string>

namespace dorm_energy::mqtt
{
    class IMqttConnection
    {
    public:
        virtual ~IMqttConnection() = default;

        virtual bool connect(
            const std::string &broker,
            const std::string &clientId,
            const std::string &username,
            const std::string &password,
            bool tlsVerify) = 0;

        virtual bool isConnected() const = 0;

        virtual bool start() = 0;

        virtual void stop() = 0;
    };
}
#pragma once

#include <string>
#include <vector>

namespace dorm_energy::mqtt
{
    class IMqttSubscription
    {
    public:
        virtual ~IMqttSubscription() = default;

        virtual void subscribe(
            const std::string &topic) = 0;

        virtual void subscribe(
            const std::vector<std::string> &topics) = 0;

        virtual void unsubscribe(
            const std::string &topic) = 0;
    };
}
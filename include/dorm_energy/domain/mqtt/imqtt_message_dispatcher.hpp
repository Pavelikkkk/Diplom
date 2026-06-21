#pragma once

#include "dorm_energy/application/imessage_handler.hpp"

#include <memory>

namespace dorm_energy::mqtt
{
    class IMqttMessageDispatcher
    {
    public:
        virtual ~IMqttMessageDispatcher() = default;

        virtual void setHandler(
            std::unique_ptr<application::IMessageHandler> handler) = 0;
    };
}
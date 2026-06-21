#pragma once

#include "dorm_energy/application/imessage_handler.hpp"
#include "dorm_energy/domain/mqtt/imqtt_connection.hpp"
#include "dorm_energy/domain/mqtt/imqtt_message_dispatcher.hpp"
#include "dorm_energy/domain/mqtt/imqtt_subscription.hpp"

#include <memory>
#include <string>
#include <vector>

namespace dorm_energy::mqtt
{
    class MqttClient final
        : public IMqttConnection,
          public IMqttSubscription,
          public IMqttMessageDispatcher
    {
    public:
        MqttClient();

        ~MqttClient() override;

        bool connect(
            const std::string &broker,
            const std::string &clientId,
            const std::string &username,
            const std::string &password,
            bool tlsVerify) override;

        bool isConnected() const override;

        bool start() override;

        void stop() override;

        void subscribe(
            const std::string &topic) override;

        void subscribe(
            const std::vector<std::string> &topics) override;

        void unsubscribe(
            const std::string &topic) override;

        void setHandler(
            std::unique_ptr<application::IMessageHandler> handler) override;

    private:
        class Impl;

        std::unique_ptr<Impl> pimpl_;
    };
}
// include/dorm_energy/infrastructure/mqtt/mqtt_client.hpp
#pragma once

#include "dorm_energy/application/imessage_handler.hpp"
#include "dorm_energy/domain/mqtt/imqtt_connection.hpp"
#include "dorm_energy/domain/mqtt/imqtt_message_dispatcher.hpp"
#include "dorm_energy/domain/mqtt/imqtt_subscription.hpp"

#include <atomic>
#include <memory>
#include <string>

namespace dorm_energy::mqtt
{

    class MqttClient final : public IMqttConnection,
                             public IMqttSubscription,
                             public IMqttMessageDispatcher
    {
    public:
        MqttClient();
        ~MqttClient() override;

        bool connect(const std::string &broker = "tcp://127.0.0.1:1883",
                     const std::string &clientId = "dorm-energy",
                     const std::string &username = "",
                     const std::string &password = "",
                     bool tlsVerify = false) override;

        bool isConnected() const override;

        bool start() override;
        void stop() override;
        void setMode(MqttMode mode) override;

        void subscribe(const std::string &topic) override;
        void subscribe(const std::vector<std::string> &topics) override;
        void unsubscribe(const std::string &topic) override;

        void setHandler(std::unique_ptr<application::IMessageHandler> handler) override;
        void setMessageCallback(MessageCallback callback) override;

    private:
        class Impl;
        std::unique_ptr<Impl> pimpl_;
    };

} // namespace dorm_energy::mqtt

#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::mqtt
{
    class MqttClient;
    class IMqttConnection;
    class IMqttSubscription;
    class IMqttMessageDispatcher;
} // namespace dorm_energy::mqtt

namespace dorm_energy::application::factories
{
    class MqttFactory
    {
    public:
        explicit MqttFactory(
            const AppConfig &config);

        std::shared_ptr<mqtt::IMqttConnection> createConnection();
        std::shared_ptr<mqtt::IMqttSubscription> createSubscription();
        std::shared_ptr<mqtt::IMqttMessageDispatcher> createDispatcher();

    private:
        const AppConfig &config_;
        
        std::shared_ptr<mqtt::MqttClient> mqttClient_{};
        std::shared_ptr<mqtt::MqttClient> createClient();
    };
} // namespace dorm_energy::application::factories

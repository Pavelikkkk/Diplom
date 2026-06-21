#include "dorm_energy/application/factories/mqtt_factory.hpp"

#include "dorm_energy/infrastructure/mqtt/mqtt_client.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    MqttFactory::MqttFactory(
        const AppConfig &config) 
        : config_(config) {}

    std::shared_ptr<mqtt::MqttClient> MqttFactory::createClient()
    {
        if (!mqttClient_)
        {
            mqttClient_ = std::make_shared<mqtt::MqttClient>();
        }

        return mqttClient_;
    }

    std::shared_ptr<mqtt::IMqttConnection> MqttFactory::createConnection()
    {
        return createClient();
    }

    std::shared_ptr<mqtt::IMqttSubscription> MqttFactory::createSubscription()
    {
        return createClient();
    }

    std::shared_ptr<mqtt::IMqttMessageDispatcher> MqttFactory::createDispatcher()
    {
        return createClient();
    }
} // namespace dorm_energy::application::factories

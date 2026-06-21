#include "dorm_energy/application/commands/daemon_command.hpp"

#include "dorm_energy/application/runtime.hpp"
#include "dorm_energy/infrastructure/web/server/web_server.hpp"

#include <chrono>
#include <fmt/format.h>
#include <stdexcept>
#include <thread>
#include <utility>

namespace dorm_energy::application
{
    DaemonCommand::DaemonCommand(
        std::shared_ptr<dorm_energy::logging::ILogger> logger,
        MqttConfig mqttConfig,
        std::shared_ptr<dorm_energy::mqtt::IMqttConnection> mqttConnection,
        std::shared_ptr<dorm_energy::mqtt::IMqttSubscription> mqttSubscription,
        std::shared_ptr<dorm_energy::mqtt::IMqttMessageDispatcher> mqttDispatcher,
        std::unique_ptr<application::IMessageHandler> messageHandler,
        std::shared_ptr<dorm_energy::web::WebServer> webServer)
        : mqttConfig_(std::move(mqttConfig)),
          logger_(std::move(logger)),
          mqtt_connection_(std::move(mqttConnection)),
          mqtt_subscription_(std::move(mqttSubscription)),
          mqtt_dispatcher_(std::move(mqttDispatcher)),
          message_handler_(std::move(messageHandler)),
          web_server_(std::move(webServer))
    {
        if (!logger_)
        {
            throw std::invalid_argument("DaemonCommand: logger must not be null");
        }

        if (!mqtt_connection_)
        {
            throw std::invalid_argument("DaemonCommand: mqttConnection must not be null");
        }

        if (!mqtt_subscription_)
        {
            throw std::invalid_argument("DaemonCommand: mqttSubscription must not be null");
        }

        if (!mqtt_dispatcher_)
        {
            throw std::invalid_argument("DaemonCommand: mqttDispatcher must not be null");
        }

        if (!message_handler_)
        {
            throw std::invalid_argument("DaemonCommand: messageHandler must not be null");
        }

        if (!web_server_)
        {
            throw std::invalid_argument("DaemonCommand: webServer must not be null");
        }
    }

    bool DaemonCommand::canHandle(
        const cli::ParsedCommand &options) const
    {
        return options.type == cli::CommandType::Daemon;
    }

    int DaemonCommand::execute()
    {
        logger_->info(
            "Launching Dorm Energy Daemon...");

        Runtime::init();

        mqtt_dispatcher_->setHandler(
            std::move(message_handler_));

        logger_->info(
            fmt::format(
                "Connecting to MQTT broker: {} as {} | auth: {} | tls verify: {}",
                mqttConfig_.broker,
                mqttConfig_.clientId,
                mqttConfig_.username.empty() ? "disabled" : "enabled",
                mqttConfig_.tlsVerify ? "enabled" : "disabled"));

        const bool connected =
            mqtt_connection_->connect(
                mqttConfig_.broker,
                mqttConfig_.clientId,
                mqttConfig_.username,
                mqttConfig_.password,
                mqttConfig_.tlsVerify);

        if (!connected)
        {
            logger_->error(
                "Failed to connect to MQTT broker");

            shutdown();

            return 1;
        }

        logger_->info(
            "MQTT connection established");

        mqtt_subscription_->subscribe(
            mqttConfig_.topic);

        logger_->info(
            fmt::format(
                "Subscribed to MQTT topic: {}",
                mqttConfig_.topic));

        if (!mqtt_connection_->start())
        {
            logger_->error(
                "Failed to start MQTT client");

            shutdown();

            return 1;
        }

        logger_->info(
            "MQTT client started");

        web_server_->start();

        logger_->info(
            "Web server started");

        logger_->info(
            "Daemon successfully started. Waiting for messages...");

        while (Runtime::isRunning())
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds{300});
        }

        shutdown();

        return 0;
    }

    void DaemonCommand::shutdown()
    {
        if (stopped_)
        {
            return;
        }

        stopped_ = true;

        logger_->info(
            "Performing graceful shutdown...");

        if (mqtt_connection_)
        {
            mqtt_connection_->stop();
        }

        if (web_server_)
        {
            web_server_->stop();
        }

        logger_->info(
            "Daemon shutdown completed.");
    }
} // namespace dorm_energy::application

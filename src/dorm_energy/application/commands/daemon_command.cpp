#include "dorm_energy/application/commands/daemon_command.hpp"
#include "dorm_energy/application/runtime.hpp"
#include "dorm_energy/infrastructure/web/server/web_server.hpp"

#include <chrono>
#include <fmt/format.h>
#include <thread>

namespace dorm_energy::application
{

    DaemonCommand::DaemonCommand(
        std::shared_ptr<dorm_energy::logging::ILogger> logger, AppConfig config,
        std::shared_ptr<dorm_energy::mqtt::IMqttConnection> mqtt_connection,
        std::shared_ptr<dorm_energy::mqtt::IMqttSubscription> mqtt_subscription,
        std::shared_ptr<dorm_energy::mqtt::IMqttMessageDispatcher> mqtt_dispatcher,
        std::unique_ptr<application::IMessageHandler> message_handler,
        std::shared_ptr<dorm_energy::web::WebServer> web_server)
        : config_(std::move(config)), logger_(std::move(logger)),
          mqtt_connection_(std::move(mqtt_connection)),
          mqtt_subscription_(std::move(mqtt_subscription)),
          mqtt_dispatcher_(std::move(mqtt_dispatcher)),
          message_handler_(std::move(message_handler)), web_server_(std::move(web_server))
    {
        if (!logger_ || !mqtt_connection_ || !mqtt_subscription_ || !mqtt_dispatcher_ ||
            !message_handler_ || !web_server_)
        {
            throw std::invalid_argument("DaemonCommand: all dependencies must be provided");
        }
    }

    int DaemonCommand::execute(const cli::CommandOptions &options)
    {
        logger_->info("Launching Dorm Energy Daemon (MQTT listener)...");

        web_server_->start();
        Runtime::setOnStopCallback(
            [this]()
            {
                logger_->info("Stop signal received. Performing graceful shutdown...");

                if (message_handler_)
                {
                    message_handler_->flush();
                }

                if (mqtt_connection_)
                {
                    mqtt_connection_->stop();
                }

                if (web_server_)
                {
                    web_server_->stop();
                }

                logger_->info("Daemon shutdown completed.");
            });

        Runtime::init();

        std::string broker = !options.mqttBroker.empty() ? options.mqttBroker : config_.getMqttBroker();
        std::string topic = !options.mqttTopic.empty() ? options.mqttTopic : config_.getMqttTopic();

        logger_->info(fmt::format("Connecting to MQTT broker: {}", broker));

        bool connected = mqtt_connection_->connect(broker, "dorm-energy-daemon");
        if (!connected)
        {
            logger_->error("Failed to connect to MQTT broker");
            return 1;
        }

        mqtt_dispatcher_->setHandler(std::move(message_handler_));
        mqtt_subscription_->subscribe(topic);

        logger_->info(fmt::format("Subscribed to topic: {}", topic));

        if (!mqtt_connection_->start())
        {
            logger_->error("Failed to start MQTT client");
            return 1;
        }

        logger_->info(
            "Daemon successfully started and connected. Waiting for messages... (Ctrl+C to stop)");

        while (Runtime::isRunning())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }

        mqtt_connection_->stop();
        web_server_->stop();

        return 0;
    }

} // namespace dorm_energy::application

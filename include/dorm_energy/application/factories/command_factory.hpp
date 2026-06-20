#pragma once

#include "dorm_energy/application/cli/parsed_command.hpp"
#include "dorm_energy/application/commands/icommand.hpp"
#include "dorm_energy/application/config/app_config.hpp"
#include "dorm_energy/application/factories/auth_factory.hpp"
#include "dorm_energy/application/factories/detection_factory.hpp"
#include "dorm_energy/application/factories/message_handler_factory.hpp"
#include "dorm_energy/application/factories/mqtt_factory.hpp"
#include "dorm_energy/application/factories/notification_factory.hpp"
#include "dorm_energy/application/factories/repository_factory.hpp"
#include "dorm_energy/application/factories/simulation_factory.hpp"
#include "dorm_energy/application/factories/state_factory.hpp"
#include "dorm_energy/application/factories/web_server_factory.hpp"
#include "dorm_energy/domain/logging/ilogger.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    namespace logging = dorm_energy::logging;

    class CommandFactory
    {
    public:
        explicit CommandFactory(
            const AppConfig &config,
            std::shared_ptr<logging::ILogger> logger);

        std::unique_ptr<ICommand> createCommand(
            const cli::ParsedCommand &options);

    private:
        const AppConfig &config_;
        std::shared_ptr<logging::ILogger> logger_;

        AuthFactory authFactory_;
        DetectionFactory detectionFactory_;
        MessageHandlerFactory messageHandlerFactory_;
        MqttFactory mqttFactory_;
        NotificationFactory notificationFactory_;
        RepositoryFactory repositoryFactory_;
        SimulationFactory simulationFactory_;
        StateFactory stateFactory_;
        WebServerFactory webServerFactory_;
    };
} // namespace dorm_energy::application::factories

#include "dorm_energy/application/application.hpp"
#include "dorm_energy/application/cli/command_options.hpp"
#include "dorm_energy/application/cli/command_type.hpp"

#include <fmt/format.h>
#include <iostream>
#include <memory>
#include <utility>
#include <string_view>

namespace dorm_energy::application
{
    namespace
    {
        std::string_view commandName(cli::CommandType type)
        {
            switch (type)
            {
            case cli::CommandType::Simulate:
                return "simulate";
            case cli::CommandType::Daemon:
                return "daemon";
            case cli::CommandType::Help:
                return "help";
            case cli::CommandType::Unknown:
            default:
                return "unknown";
            }
        }

        std::string_view enabled(bool value)
        {
            return value ? "enabled" : "disabled";
        }
    }

    Application::Application(
        std::shared_ptr<const AppConfig> config,
        std::shared_ptr<logging::ILogger> logger,
        std::unique_ptr<cli::ICliParser> cliParser,
        std::unique_ptr<factories::CommandFactory> commandFactory)
        : config_(std::move(config)),
          logger_(std::move(logger)),
          cliParser_(std::move(cliParser)),
          commandFactory_(std::move(commandFactory))
    {
    }

    int Application::run(int argc, char **argv)
    {
        cli::CommandOptions options;
        const cli::ParseResult parseResult = cliParser_->parse(argc, argv, options);

        if (parseResult != cli::ParseResult::Continue)
        {
            return (parseResult == cli::ParseResult::ExitSuccess) ? 0 : 1;
        }

        if (options.type != cli::CommandType::Help)
        {
            logEffectiveConfig(options);
        }

        auto command = commandFactory_->createCommand(options);

        if (command && command->canHandle(options))
        {
            return command->execute(options);
        }

        logger_->error("Unknown command. Use help for available commands.");
        return 1;
    }

    void Application::logEffectiveConfig(const cli::CommandOptions &options) const
    {
        logger_->info(fmt::format("Command: {}",
                                  commandName(options.type)));

        logger_->info(fmt::format("Config log level: {} | verbose: {}",
                                  config_->getLogLevel(), enabled(config_->isVerbose())));

        if (options.type == cli::CommandType::Simulate)
        {
            logger_->info(fmt::format(
                "Config simulation days: {} | random seed: {} | anomalies: {} | anomaly rate: {}",
                config_->getSimulationDays(), config_->getRandomSeed(), enabled(config_->getInjectAnomalies()), config_->getAnomalyRate()));
        }

        if (options.type == cli::CommandType::Daemon)
        {
            logger_->info(fmt::format("Config MQTT broker: {} | client id: {} | topic: {} | auth: {} | tls verify: {}",
                                      config_->getMqttBroker(), config_->getMqttClientId(),
                                      config_->getMqttTopic(),
                                      enabled(!config_->getMqttUsername().empty()),
                                      enabled(config_->getMqttTlsVerify())));

            logger_->info(fmt::format("Config Telegram: {} | chat id: {}",
                                      enabled(config_->isTelegramEnabled()), config_->getTelegramChatId()));
        }

        logger_->info(fmt::format("Config database: {}:{}/{} as {} | buffer: {}",
                                  config_->getDbHost(), config_->getDbPort(), config_->getDbName(), config_->getDbUser(), config_->getDbMaxBufferSize()));
    }
} // namespace dorm_energy::application

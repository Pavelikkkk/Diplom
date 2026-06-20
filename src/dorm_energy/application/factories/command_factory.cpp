#include "dorm_energy/application/factories/command_factory.hpp"

#include "dorm_energy/application/cli/command_type.hpp"
#include "dorm_energy/application/commands/daemon_command.hpp"
#include "dorm_energy/application/commands/simulate_command.hpp"
#include "dorm_energy/application/inotifier.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application::factories
{
    CommandFactory::CommandFactory(
        const AppConfig &config,
        std::shared_ptr<logging::ILogger> logger)
        : config_(config),
          logger_(std::move(logger)),
          authFactory_(config_),
          detectionFactory_(config_),
          mqttFactory_(config_),
          notificationFactory_(config_),
          repositoryFactory_(config_),
          simulationFactory_(config_),
          stateFactory_(config_)
    {
    }

    std::unique_ptr<ICommand> CommandFactory::createCommand(const cli::ParsedCommand &options)
    {
        if (options.type == cli::CommandType::Simulate)
        {
            return std::make_unique<SimulateCommand>(
                logger_,
                config_,
                simulationFactory_.createGenerator(),
                detectionFactory_.create());
        }

        if (options.type == cli::CommandType::Daemon)
        {
            auto repository = repositoryFactory_.create();
            auto aggregator = stateFactory_.createAggregator();
            auto authService = authFactory_.create(repository);
            auto webServer = webServerFactory_.create(aggregator, repository, authService);

            auto messageHandler = messageHandlerFactory_.create(
                detectionFactory_.create(),
                repository,
                notificationFactory_.create(),
                aggregator);

            return std::make_unique<DaemonCommand>(
                logger_,
                config_,
                mqttFactory_.createConnection(),
                mqttFactory_.createSubscription(),
                mqttFactory_.createDispatcher(),
                std::move(messageHandler),
                webServer);
        }

        return nullptr;
    }
} // namespace dorm_energy::application::factories

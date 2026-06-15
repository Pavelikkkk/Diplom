#include "dorm_energy/application/application_builder.hpp"

#include "dorm_energy/application/application.hpp"
#include "dorm_energy/application/imessage_handler.hpp"
#include "dorm_energy/application/inotifier.hpp"

#include <memory>
#include <utility>

namespace dorm_energy::application
{
    ApplicationBuilder &ApplicationBuilder::withConfig(AppConfig config)
    {
        config_ = std::move(config);
        return *this;
    }

    std::unique_ptr<Application> ApplicationBuilder::build()
    {
        config_.validate();

        auto repository = repositoryFactory_.create();
        auto aggregator = stateFactory_.createAggregator();

        auto cliParser = cliFactory_.createParser();

        auto simulateCommand = commandFactory_.createSimulateCommand(
            loggerFactory_.create(),
            simulationFactory_.createGenerator(repository),
            detectionFactory_.create(), repository);

        auto authService = authFactory_.create(repository);
        auto webServer = webServerFactory_.create(aggregator, repository, authService);

        auto messageHandler = messageHandlerFactory_.create(
            detectionFactory_.create(),
            repository, notificationFactory_.create(),
            aggregator);

        auto daemonCommand = commandFactory_.createDaemonCommand(
            loggerFactory_.create(),
            mqttFactory_.createConnection(),
            mqttFactory_.createSubscription(),
            mqttFactory_.createDispatcher(),
            std::move(messageHandler),
            webServer);

        return std::make_unique<Application>(config_, std::move(cliParser),
                                             std::move(simulateCommand), std::move(daemonCommand),
                                             repository);
    }
} // namespace dorm_energy::application

#include "dorm_energy/application/application_builder.hpp"

#include "dorm_energy/application/application.hpp"
#include "dorm_energy/application/factories/logger_factory.hpp"

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

        auto appConfig = std::make_shared<const AppConfig>(config_);
        auto cliParser = cliFactory_.createParser();

        factories::LoggerFactory loggerFactory{*appConfig};
        auto logger = loggerFactory.create();
        
        auto commandFactory = std::make_unique<factories::CommandFactory>(*appConfig, logger);

        return std::make_unique<Application>(
            std::move(appConfig),
            std::move(logger),
            std::move(cliParser),
            std::move(commandFactory));
    }
} // namespace dorm_energy::application

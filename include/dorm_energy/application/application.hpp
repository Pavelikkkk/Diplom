#pragma once

#include "dorm_energy/application/cli/icli_parser.hpp"
#include "dorm_energy/application/config/app_config.hpp"
#include "dorm_energy/application/factories/command_factory.hpp"
#include "dorm_energy/domain/logging/ilogger.hpp"

#include <memory>

namespace dorm_energy::application
{
    class Application
    {
    public:
        explicit Application(
            std::shared_ptr<const AppConfig> config,
            std::shared_ptr<logging::ILogger> logger,
            std::unique_ptr<cli::ICliParser> cliParser,
            std::unique_ptr<factories::CommandFactory> commandFactory);

        ~Application() = default;

        int run(int argc, char **argv);

    private:
        void logEffectiveConfig( // лучше как хелпер в cpp 
            const cli::ParsedCommand &options) const;

        std::shared_ptr<const AppConfig> config_;
        std::shared_ptr<logging::ILogger> logger_;
        std::unique_ptr<cli::ICliParser> cliParser_;
        std::unique_ptr<factories::CommandFactory> commandFactory_;
    };
} // namespace dorm_energy::application

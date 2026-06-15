#include "dorm_energy/application/application.hpp"
#include "dorm_energy/application/cli/command_options.hpp"
#include "dorm_energy/application/config/app_config.hpp"

#include <iostream>
#include <memory>
#include <utility>

namespace dorm_energy::application
{
    Application::Application(
        AppConfig config,
        std::unique_ptr<cli::CliParser> cliParser,
        std::unique_ptr<SimulateCommand> simulateCommand,
        std::unique_ptr<DaemonCommand> daemonCommand,
        std::shared_ptr<storage::IMeasurementRepository> repository)
        : config_(std::move(config)),
          cliParser_(std::move(cliParser)),
          simulateCommand_(std::move(simulateCommand)),
          daemonCommand_(std::move(daemonCommand)),
          repository_(std::move(repository))
    {
    }

    int Application::run(int argc, char **argv)
    {
        cli::CommandOptions options;
        cli::ParseResult parseResult = cliParser_->parse(argc, argv, options);

        if (parseResult != cli::ParseResult::Continue)
        {
            return (parseResult == cli::ParseResult::ExitSuccess) ? 0 : 1;
        }

        config_.overrideFromCli(options);

        config_.validate();

        if (options.isSimulate())
        {
            return simulateCommand_->execute(options);
        }
        else if (options.isDaemon())
        {
            return daemonCommand_->execute(options);
        }
        else if (options.isHelp())
        {
            return 0;
        }
        else
        {
            std::cerr << "Unknown command. Use --help for help.\n";
            return 1;
        }
    }

} // namespace dorm_energy::application
#include "dorm_energy/infrastructure/cli/cli_parser.hpp"
#include "dorm_energy/application/cli/command_options.hpp"
#include "dorm_energy/application/cli/command_type.hpp"

#include <CLI/CLI.hpp>
#include <iostream>

namespace dorm_energy::cli
{
    CliParser::CliParser()
        : app_(std::make_unique<CLI::App>(
              "System of smart monitoring"))
    {
        app_->name("SoSM");                      // поменять потом в exec cmake
        app_->get_formatter()->column_width(25); //
        setupCommands();
    }

    void CliParser::setupCommands()
    {
        app_->require_subcommand(0, 1);
        app_->add_subcommand("simulate", "Run data generation simulation using AppConfig");
        app_->add_subcommand("daemon", "Run as a daemon using AppConfig");
        app_->add_subcommand("help", "Show help message");
    }

    ParseResult CliParser::parse(int argc, char **argv, CommandOptions &options)
    {
        try
        {
            app_->parse(argc, argv);

            if (app_->get_subcommand("simulate")->parsed())
            {
                options.type = CommandType::Simulate;
                return ParseResult::Continue;
            }
            else if (app_->get_subcommand("daemon")->parsed())
            {
                options.type = CommandType::Daemon;
                return ParseResult::Continue;
            }
            else if (app_->get_subcommand("help")->parsed() || !app_->get_subcommand()->parsed())
            {
                options.type = CommandType::Help; // убрать
                std::cout << app_->help() << '\n';
                return ParseResult::ExitSuccess;
            }
        }
        catch (const CLI::ParseError &e)
        {
            app_->exit(e);
            return ParseResult::ExitFailure;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Argument parsing error: " << e.what() << std::endl; //
            return ParseResult::ExitFailure;
        }
    }
} // namespace dorm_energy::cli

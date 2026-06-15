#include "dorm_energy/infrastructure/cli/cli_parser.hpp"
#include "dorm_energy/application/cli/command_options.hpp"
#include "dorm_energy/application/cli/command_type.hpp"

#include <CLI/CLI.hpp>
#include <iostream>

namespace dorm_energy::cli
{
    CliParser::CliParser()
        : app_(std::make_unique<CLI::App>(
              "Dorm Energy Monitor - monitoring of energy consumption in the dormitory"))
    {
        app_->name("dorm-sim");
        app_->get_formatter()->column_width(25);
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
            }
            else if (app_->get_subcommand("daemon")->parsed())
            {
                options.type = CommandType::Daemon;
            }
            else if (app_->get_subcommand("help")->parsed() || !app_->get_subcommand()->parsed())
            {
                options.type = CommandType::Help;
                std::cout << app_->help() << std::endl;
                return ParseResult::ExitSuccess;
            }

            return ParseResult::Continue;
        }
        catch (const CLI::ParseError &e)
        {
            app_->exit(e);
            return ParseResult::ExitError;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Argument parsing error: " << e.what() << std::endl;
            return ParseResult::ExitError;
        }
    }
} // namespace dorm_energy::cli

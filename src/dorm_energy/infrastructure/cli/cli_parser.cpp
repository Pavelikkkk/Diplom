#include "dorm_energy/infrastructure/cli/cli_parser.hpp"

#include "dorm_energy/application/cli/command_type.hpp"
#include "dorm_energy/application/cli/parsed_command.hpp"

#include <CLI/CLI.hpp>
#include <iostream>

namespace dorm_energy::cli
{
    CliParser::CliParser()
        : app_(std::make_unique<CLI::App>("System of smart monitoring"))
    {
        app_->name("SoSM");
        app_->get_formatter()->column_width(25);
        setupCommands();
    }

    void CliParser::setupCommands()
    {
        app_->require_subcommand(0, 1);
        app_->add_subcommand("simulate", "Run offline simulation using AppConfig");
        app_->add_subcommand("daemon", "Run as a daemon using AppConfig");
        app_->add_subcommand("help", "Show help message");
    }

    ParseResult CliParser::parse(int argc, char **argv, ParsedCommand &command)
    {
        try
        {
            app_->parse(argc, argv);

            if (app_->get_subcommand("simulate")->parsed())
            {
                command.type = CommandType::Simulate;
                return ParseResult::Continue;
            }

            if (app_->get_subcommand("daemon")->parsed())
            {
                command.type = CommandType::Daemon;
                return ParseResult::Continue;
            }

            if (app_->get_subcommand("help")->parsed() || !app_->get_subcommand()->parsed())
            {
                command.type = CommandType::Help;
                std::cout << app_->help() << '\n';
                return ParseResult::ExitSuccess;
            }

            command.type = CommandType::Unknown;
            return ParseResult::ExitFailure;
        }
        catch (const CLI::ParseError &e)
        {
            app_->exit(e);
            return ParseResult::ExitFailure;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Argument parsing error: " << e.what() << std::endl;
            return ParseResult::ExitFailure;
        }
    }
} // namespace dorm_energy::cli

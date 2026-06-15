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
        app_->add_flag("-v,--verbose", "Enable verbose output");

        auto *simulate = app_->add_subcommand("simulate", "Run data generation simulation");
        simulate->add_option("--days", "Number of days to simulate")
            ->default_val(30)
            ->capture_default_str();

        simulate->add_flag("--anomalies", "Inject anomalies into the data");
        simulate->add_option("--anomaly-rate", "Anomaly injection rate (0.0 - 1.0)")
            ->default_val(0.03)
            ->capture_default_str();

        auto *daemon = app_->add_subcommand("daemon", "Run as a daemon (MQTT listener)");

        daemon->add_option("--mqtt-broker", "MQTT broker address (example: tcp://127.0.0.1:1883)")
            ->default_val("tcp://127.0.0.1:1883")
            ->capture_default_str();

        daemon->add_option("--mqtt-topic", "MQTT topic to subscribe")
            ->default_val("devices/+/+") // поменять на знач из конфигов
            ->capture_default_str();

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
                auto *daemon = app_->get_subcommand("daemon");

                options.mqttBroker = daemon->get_option("--mqtt-broker")->as<std::string>();
                options.mqttTopic = daemon->get_option("--mqtt-topic")->as<std::string>();
            }
            else if (app_->get_subcommand("help")->parsed() || !app_->get_subcommand()->parsed())
            {
                options.type = CommandType::Help;
                std::cout << app_->help() << std::endl;

                return ParseResult::ExitSuccess;
            }

            options.common.verbose = app_->get_option("--verbose")->count() > 0;

            if (options.isSimulate())
            {
                auto *simulate = app_->get_subcommand("simulate");
                options.simulateDays = simulate->get_option("--days")->as<int>();
                options.injectAnomalies = simulate->get_option("--anomalies")->count() > 0;
                options.anomalyRate = simulate->get_option("--anomaly-rate")->as<double>();
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
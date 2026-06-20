// include/dorm_energy/infrastructure/cli/cli_parser.hpp
#pragma once

#include "dorm_energy/application/cli/icli_parser.hpp"

#include <CLI/CLI.hpp>

#include <memory>

namespace dorm_energy::cli
{
    
    class CliParser : public ICliParser
    {
    public:
        CliParser();

        ParseResult parse(int argc, char **argv, ParsedCommand &options) override;

    private:
        void setupCommands();
        std::unique_ptr<CLI::App> app_;
    };

} // namespace dorm_energy::cli

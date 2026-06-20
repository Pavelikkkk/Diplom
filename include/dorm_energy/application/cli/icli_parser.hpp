#pragma once

#include "dorm_energy/application/cli/parsed_command.hpp"
#include "dorm_energy/application/cli/parse_result.hpp"

namespace dorm_energy::cli
{
    class ICliParser
    {
    public:
        virtual ~ICliParser() = default;

        virtual ParseResult parse(int argc, char **argv, ParsedCommand &options) = 0;
    };
} // namespace dorm_energy::cli

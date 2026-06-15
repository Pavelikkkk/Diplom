#pragma once

#include "dorm_energy/application/cli/command_options.hpp"
#include "dorm_energy/application/cli/parse_result.hpp"

namespace dorm_energy::cli
{
    class ICliParser
    {
    public:
        virtual ~ICliParser() = default;

        virtual ParseResult parse(int argc, char **argv, CommandOptions &options) = 0;
    };
} // namespace dorm_energy::cli

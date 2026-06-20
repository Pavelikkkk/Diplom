#pragma once

#include "dorm_energy/application/cli/parsed_command.hpp"

namespace dorm_energy::application
{
    class ICommand
    {
    public:
        virtual ~ICommand() = default;

        virtual bool canHandle(const cli::ParsedCommand &options) const = 0;
        virtual int execute() = 0;
    };
} // namespace dorm_energy::application

#pragma once

#include "dorm_energy/application/cli/command_options.hpp"

namespace dorm_energy::application
{
    class ICommand
    {
    public:
        virtual ~ICommand() = default;

        virtual bool canHandle(const cli::CommandOptions &options) const = 0;
        virtual int execute(const cli::CommandOptions &options) = 0;
    };
} // namespace dorm_energy::application

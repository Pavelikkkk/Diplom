#pragma once

#include "command_type.hpp"

namespace dorm_energy::cli
{
    struct CommandOptions
    {
        CommandType type{CommandType::Unknown};
    };
} // namespace dorm_energy::cli

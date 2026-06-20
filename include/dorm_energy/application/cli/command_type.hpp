#pragma once

namespace dorm_energy::cli
{

    enum class CommandType
    {
        Unknown,
        Help, // поменять название 
        Simulate,
        Daemon
    };

} // namespace dorm_energy::cli
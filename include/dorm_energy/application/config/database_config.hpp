#pragma once

#include <cstddef>
#include <string>

namespace dorm_energy::application
{
    struct DatabaseConfig
    {
        std::string host{};
        std::string port{"5432"};
        std::string name{};
        std::string user{};
        std::string password{};
        std::size_t maxBufferSize{2000};
    };
} // namespace dorm_energy::application

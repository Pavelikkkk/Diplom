#pragma once

#include "dorm_energy/application/cli/icli_parser.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    class CliFactory
    {
    public:
        std::unique_ptr<cli::ICliParser> createParser() const;
    };
} // namespace dorm_energy::application::factories

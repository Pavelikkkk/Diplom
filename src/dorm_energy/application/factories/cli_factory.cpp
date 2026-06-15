#include "dorm_energy/application/factories/cli_factory.hpp"

#include "dorm_energy/infrastructure/cli/cli_parser.hpp"

#include <memory>

namespace dorm_energy::application::factories
{
    std::unique_ptr<cli::ICliParser> CliFactory::createParser() const
    {
        return std::make_unique<cli::CliParser>();
    }
} // namespace dorm_energy::application::factories

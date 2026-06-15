#include "dorm_energy/application/factories/repository_factory.hpp"

#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"

#include <exception>
#include <iostream>
#include <memory>
#include <string>

namespace dorm_energy::application::factories
{
    RepositoryFactory::RepositoryFactory(
        const AppConfig &config)
        : config_(config) {}

    std::shared_ptr<storage::IMeasurementRepository> RepositoryFactory::create()
    {
        if (repository_)
        {
            return repository_;
        }

        try
        {
            const std::string connStr = config_.getDbConnectionString();

            repository_ = std::make_shared<storage::PostgresMeasurementRepository>(connStr, config_.getDbMaxBufferSize());

            return repository_;
        }
        catch (const std::exception &e)
        {
            std::cerr << "[RepositoryFactory] Failed to create repository: " << e.what() << '\n';
            throw;
        }
    }
} // namespace dorm_energy::application::factories

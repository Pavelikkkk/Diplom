#include "dorm_energy/application/factories/repository_factory.hpp"

#include "dorm_energy/infrastructure/storage/postgres_repository.hpp"

#include <memory>
#include <string>

namespace dorm_energy::application::factories
{
    RepositoryFactory::RepositoryFactory(
        const AppConfig &config)
        : config_(config)
    {
    }

    std::shared_ptr<storage::IMeasurementRepository> RepositoryFactory::create()
    {
        if (repository_)
        {
            return repository_;
        }

        const std::string connStr = config_.getDbConnectionString();

        repository_ = std::make_shared<storage::PostgresMeasurementRepository>(connStr, config_.getDbMaxBufferSize());

        return repository_;
    }
} // namespace dorm_energy::application::factories
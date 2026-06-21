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

    RepositoryPorts RepositoryFactory::create()
    {
        auto repository = createPostgresRepository();

        return RepositoryPorts{
            .measurements = repository,
            .anomalies = repository,
            .dashboard = repository,
            .catalog = repository,
            .users = repository,
            .subscriptions = repository,
            .admin = repository,
        };
    }

    std::shared_ptr<storage::PostgresMeasurementRepository> RepositoryFactory::createPostgresRepository()
    {
        if (repository_)
        {
            return repository_;
        }

        const auto dbConfig = config_.getDatabaseConfig();
        const std::string connStr = config_.getDbConnectionString();

        repository_ = std::make_shared<storage::PostgresMeasurementRepository>(connStr, dbConfig.maxBufferSize);

        return repository_;
    }
} // namespace dorm_energy::application::factories

#pragma once

#include "dorm_energy/application/commands/icommand.hpp"
#include "dorm_energy/application/config/app_config.hpp"
#include "dorm_energy/domain/detection/istate_detector.hpp"
#include "dorm_energy/domain/logging/ilogger.hpp"
#include "dorm_energy/domain/simulation/idata_generator.hpp"
#include "dorm_energy/domain/storage/imeasurement_repository.hpp"

#include <memory>

namespace dorm_energy::application
{
    class SimulateCommand : public ICommand
    {
    public:
        explicit SimulateCommand(
            std::shared_ptr<dorm_energy::logging::ILogger> logger, const AppConfig &config,
            std::unique_ptr<dorm_energy::simulation::IDataGenerator> generator,
            std::unique_ptr<dorm_energy::detection::IStateDetector> detector,
            std::shared_ptr<dorm_energy::storage::IMeasurementRepository> repository);

        bool canHandle(const cli::CommandOptions &options) const override;
        int execute(const cli::CommandOptions &options) override;

    private:
        std::shared_ptr<dorm_energy::logging::ILogger> logger_;
        const AppConfig &config_;
        std::unique_ptr<dorm_energy::simulation::IDataGenerator> generator_;
        std::unique_ptr<dorm_energy::detection::IStateDetector> detector_;
        std::shared_ptr<dorm_energy::storage::IMeasurementRepository> repository_;
    };
} // namespace dorm_energy::application

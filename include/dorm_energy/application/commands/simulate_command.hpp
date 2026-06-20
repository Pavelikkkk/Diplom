#pragma once

#include "dorm_energy/application/commands/icommand.hpp"
#include "dorm_energy/application/config/app_config.hpp"
#include "dorm_energy/domain/detection/istate_detector.hpp"
#include "dorm_energy/domain/logging/ilogger.hpp"
#include "dorm_energy/domain/simulation/idata_generator.hpp"

#include <memory>

namespace dorm_energy::application
{
    class SimulateCommand : public ICommand
    {
    public:
        explicit SimulateCommand(
            std::shared_ptr<dorm_energy::logging::ILogger> logger,
            const AppConfig &config,
            std::unique_ptr<dorm_energy::simulation::IDataGenerator> generator,
            std::unique_ptr<dorm_energy::detection::IStateDetector> detector);

        bool canHandle(
            const cli::ParsedCommand &options) const override;

        int execute() override;

    private:
        std::shared_ptr<dorm_energy::logging::ILogger> logger_;
        const AppConfig &config_;
        std::unique_ptr<dorm_energy::simulation::IDataGenerator> generator_;
        std::unique_ptr<dorm_energy::detection::IStateDetector> detector_;
    };
} // namespace dorm_energy::application

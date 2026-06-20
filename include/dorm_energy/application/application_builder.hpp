#pragma once

#include "dorm_energy/application/config/app_config.hpp"
#include "dorm_energy/application/factories/cli_factory.hpp"

#include <memory>

namespace dorm_energy::application
{
    class Application;

    class ApplicationBuilder
    {
    public:
        ApplicationBuilder() = default;

        ApplicationBuilder &withConfig(
            AppConfig config);

        std::unique_ptr<Application> build();

    private:
        AppConfig config_{};
        factories::CliFactory cliFactory_{};
    };
} // namespace dorm_energy::application

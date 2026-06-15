#include "dorm_energy/application/application.hpp"
#include "dorm_energy/application/application_builder.hpp"
#include "dorm_energy/application/config/app_config.hpp"

#include <iostream>
#include <utility>

int main(int argc, char **argv)
{

    std::cout << "=== Dorm Energy Simulator ===" << std::endl;

    try
    {
        using dorm_energy::application::AppConfig;
        using dorm_energy::application::ApplicationBuilder;

        AppConfig config = AppConfig::load(); 

        auto application = ApplicationBuilder()
                               .withConfig(std::move(config))
                               .build();

        return application->run(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Critical error:\n" << e.what(); 
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred!\n"; 
        return 1;
    }
}
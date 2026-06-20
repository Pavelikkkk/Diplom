#pragma once

#include "dorm_energy/application/config/app_config.hpp"

#include <memory>

namespace dorm_energy::detection
{
    class IStateDetector;
}

namespace dorm_energy::application::factories
{
    class DetectionFactory
    {
    public:
        explicit DetectionFactory(
            const AppConfig &config);

        std::unique_ptr<detection::IStateDetector> create() const;

    private:
        const AppConfig &config_;
    };
} // namespace dorm_energy::application::factories

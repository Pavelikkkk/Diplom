#include "dorm_energy/application/runtime.hpp"

namespace dorm_energy::application
{
    void Runtime::init()
    {
        running_ = true;

        std::signal(
            SIGINT,
            Runtime::signalHandler);

        std::signal(
            SIGTERM,
            Runtime::signalHandler);
    }

    void Runtime::stop()
    {
        running_ = false;
    }

    bool Runtime::isRunning() noexcept
    {
        return running_;
    }

    void Runtime::signalHandler(
        int)
    {
        running_ = false;
    }
}
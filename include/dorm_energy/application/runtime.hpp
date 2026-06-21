#pragma once

#include <atomic>
#include <csignal>

namespace dorm_energy::application
{
    class Runtime
    {
    public:
        static void init();

        static void stop();

        static bool isRunning() noexcept;

    private:
        static inline std::atomic<bool> running_{true};

        static void signalHandler(
            int signal);
    };
}
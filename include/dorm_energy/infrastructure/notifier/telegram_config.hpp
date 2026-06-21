#pragma once

#include <cstddef>
#include <string>

namespace dorm_energy::notifier
{
    struct TelegramConfig
    {
        bool enabled{false};
        std::string botToken{};
        std::string chatId{};
        std::size_t maxQueueSize{500};
    };
} // namespace dorm_energy::notifier

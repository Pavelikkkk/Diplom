#pragma once

#include "dorm_energy/core/aliases.hpp"

#include <filesystem>
#include <string>

namespace dorm_energy::simulation::csv
{
    std::string formatTimestamp(
        core::TimePoint timestamp);

    std::string escape(
        const std::string &value);

    void ensureParentDirectory(
        const std::filesystem::path &filePath);

} // namespace dorm_energy::simulation::csv

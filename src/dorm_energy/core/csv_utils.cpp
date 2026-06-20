#include "dorm_energy/core/csv_utils.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace dorm_energy::simulation::csv
{
    std::string formatTimestamp(core::TimePoint timestamp)
    {
        const auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm localTime{};

#ifdef _WIN32
        localtime_s(&localTime, &time);
#else
        localtime_r(&time, &localTime);
#endif

        std::ostringstream oss;
        oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    std::string escape(const std::string &value)
    {
        if (value.find_first_of(",\"\n\r") == std::string::npos)
        {
            return value;
        }

        std::string escaped = "\"";
        for (const char ch : value)
        {
            if (ch == '"')
            {
                escaped += "\"\"";
            }
            else
            {
                escaped += ch;
            }
        }
        escaped += '"';
        return escaped;
    }

    void ensureParentDirectory(const std::filesystem::path &filePath)
    {
        const auto parentPath = filePath.parent_path();
        if (!parentPath.empty())
        {
            std::filesystem::create_directories(parentPath);
        }
    }
} // namespace dorm_energy::simulation::csv

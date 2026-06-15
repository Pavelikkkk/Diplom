#include "dorm_energy/application/config/app_config.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
    std::string trim(std::string value)
    {
        const auto first = value.find_first_not_of(" \t\r\n");

        if (first == std::string::npos)
            return "";

        const auto last = value.find_last_not_of(" \t\r\n");

        return value.substr(first, last - first + 1);
    }

    std::string strToLower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return value;
    }

    bool parseBool(const std::string &value)
    {
        const std::string normalized = strToLower(trim(value));

        return normalized == "1" || normalized == "true" || normalized == "on" ||
               normalized == "yes";
    }

    int parseIntOrThrow(const std::string &key, const std::string &value)
    {
        try
        {
            const std::string normalized = trim(value);

            std::size_t processedChars = 0;
            const int result = std::stoi(normalized, &processedChars);

            if (processedChars != normalized.size())
                throw std::invalid_argument("unexpected characters");

            return result;
        }
        catch (const std::exception &)
        {
            throw std::runtime_error("Invalid integer value for " + key + ": " + value);
        }
    }

    double parseDoubleOrThrow(const std::string &key, const std::string &value)
    {
        try
        {
            const std::string normalized = trim(value);

            std::size_t processedChars = 0;
            const double result = std::stod(normalized, &processedChars);

            if (processedChars != normalized.size())
                throw std::invalid_argument("unexpected characters");

            return result;
        }
        catch (const std::exception &)
        {
            throw std::runtime_error("Invalid double value for " + key + ": " + value);
        }
    }

    std::optional<std::size_t> parseSize(const std::string &value)
    {
        try
        {
            const std::string normalized = trim(value);

            std::size_t processedChars = 0;
            const unsigned long long result = std::stoull(normalized, &processedChars);

            if (processedChars != normalized.size())
                return std::nullopt;

            return static_cast<std::size_t>(result);
        }
        catch (const std::exception &)
        {
            return std::nullopt;
        }
    }

    void warnInvalidConfigValue(const std::string &key, const std::string &value,
                                const std::string &fallback)
    {
        std::cerr << "Warning: invalid " << key << " value \"" << value
                  << "\", using default/current value: " << fallback << '\n';
    }

    void applyOptionalSize(std::size_t &target, const std::string &key, const std::string &value)
    {
        if (const auto parsed = parseSize(value))
        {
            target = *parsed;
        }
        else
        {
            warnInvalidConfigValue(key, value, std::to_string(target));
        }
    }

    std::filesystem::path resolveEnvFilePath(const std::string &filename)
    {
        try
        {
            std::filesystem::path currentPath = std::filesystem::current_path();

            while (true)
            {
                const std::filesystem::path envPath = currentPath / filename;

                if (std::filesystem::exists(envPath))
                    return envPath;

                const std::filesystem::path parentPath = currentPath.parent_path();

                if (parentPath == currentPath)
                    return std::filesystem::current_path() / filename;

                currentPath = parentPath;
            }
        }
        catch (const std::filesystem::filesystem_error &)
        {
            return filename;
        }
    }
} // namespace

namespace dorm_energy::application
{
    AppConfig AppConfig::load()
    {
        AppConfig config = loadFromEnvFile(".env");

        config.overrideFromEnvironment();
        config.validate();

        return config;
    }

    AppConfig AppConfig::loadFromEnvFile(const std::string &filename)
    {
        AppConfig config;

        const std::filesystem::path envPath = resolveEnvFilePath(filename);

        std::ifstream file(envPath);

        if (!file.is_open())
            return config;

        std::string line;

        while (std::getline(file, line))
        {
            line = trim(line);

            if (line.empty() || line[0] == '#')
                continue;

            const auto pos = line.find('=');

            if (pos == std::string::npos)
                continue;

            const std::string key = trim(line.substr(0, pos));
            const std::string value = trim(line.substr(pos + 1));

            if (key.empty())
                continue;

            if (key == "LOG_LEVEL")
                config.logLevel_ = value;
            else if (key == "VERBOSE")
                config.verbose_ = parseBool(value);
            else if (key == "SIMULATION_DAYS")
                config.simulationDays_ = parseIntOrThrow(key, value);
            else if (key == "RANDOM_SEED")
                config.randomSeed_ = parseIntOrThrow(key, value);
            else if (key == "INJECT_ANOMALIES")
                config.injectAnomalies_ = parseBool(value);
            else if (key == "ANOMALY_RATE")
                config.anomalyRate_ = parseDoubleOrThrow(key, value);
            else if (key == "DB_HOST")
                config.dbHost_ = value;
            else if (key == "DB_PORT")
                config.dbPort_ = value;
            else if (key == "DB_NAME")
                config.dbName_ = value;
            else if (key == "DB_USER")
                config.dbUser_ = value;
            else if (key == "DB_PASSWORD")
                config.dbPassword_ = value;
            else if (key == "DB_MAX_BUFFER_SIZE")
                applyOptionalSize(config.dbMaxBufferSize_, key, value);
            else if (key == "MQTT_BROKER")
                config.mqttBroker_ = value;
            else if (key == "MQTT_CLIENT_ID")
                config.mqttClientId_ = value;
            else if (key == "MQTT_TOPIC")
                config.mqttTopic_ = value;
            else if (key == "TELEGRAM_ENABLED")
                config.telegramEnabled_ = parseBool(value);
            else if (key == "TELEGRAM_BOT_TOKEN")
                config.telegramBotToken_ = value;
            else if (key == "TELEGRAM_CHAT_ID")
                config.telegramChatId_ = value;
        }
        return config;
    }

    AppConfig AppConfig::loadFromEnvironment()
    {
        AppConfig config;

        config.overrideFromEnvironment();
        config.validate();

        return config;
    }

    void AppConfig::overrideFromEnvironment()
    {
        if (const char *val = std::getenv("LOG_LEVEL"))
            logLevel_ = val;
        if (const char *val = std::getenv("VERBOSE"))
            verbose_ = parseBool(val);
        if (const char *val = std::getenv("SIMULATION_DAYS"))
            simulationDays_ = parseIntOrThrow("SIMULATION_DAYS", val);

        if (const char *val = std::getenv("RANDOM_SEED"))
            randomSeed_ = parseIntOrThrow("RANDOM_SEED", val);

        if (const char *val = std::getenv("INJECT_ANOMALIES"))
            injectAnomalies_ = parseBool(val);
        if (const char *val = std::getenv("ANOMALY_RATE"))
            anomalyRate_ = parseDoubleOrThrow("ANOMALY_RATE", val);

        if (const char *val = std::getenv("DB_HOST"))
            dbHost_ = val;
        if (const char *val = std::getenv("DB_PORT"))
            dbPort_ = val;
        if (const char *val = std::getenv("DB_NAME"))
            dbName_ = val;
        if (const char *val = std::getenv("DB_USER"))
            dbUser_ = val;
        if (const char *val = std::getenv("DB_PASSWORD"))
            dbPassword_ = val;
        if (const char *val = std::getenv("DB_MAX_BUFFER_SIZE"))
            applyOptionalSize(dbMaxBufferSize_, "DB_MAX_BUFFER_SIZE", val);

        if (const char *val = std::getenv("MQTT_BROKER"))
            mqttBroker_ = val;
        if (const char *val = std::getenv("MQTT_CLIENT_ID"))
            mqttClientId_ = val;
        if (const char *val = std::getenv("MQTT_TOPIC"))
            mqttTopic_ = val;

        if (const char *val = std::getenv("TELEGRAM_ENABLED"))
            telegramEnabled_ = parseBool(val);
        if (const char *val = std::getenv("TELEGRAM_BOT_TOKEN"))
            telegramBotToken_ = val;
        if (const char *val = std::getenv("TELEGRAM_CHAT_ID"))
            telegramChatId_ = val;
    }

    std::string AppConfig::getDbConnectionString() const
    {
        std::ostringstream oss;

        oss << "host=" << dbHost_ << " port=" << dbPort_ << " dbname=" << dbName_
            << " user=" << dbUser_;

        if (!dbPassword_.empty())
            oss << " password=" << dbPassword_;

        return oss.str();
    }

    void AppConfig::validate() const
    {
        if (logLevel_.empty())
            throw std::runtime_error("LOG_LEVEL is not set");
        if (simulationDays_ <= 0)
            throw std::runtime_error("SIMULATION_DAYS must be positive");
        if (randomSeed_ < 0)
            throw std::runtime_error("RANDOM_SEED must be non-negative");
        if (anomalyRate_ < 0.0 || anomalyRate_ > 1.0)
            throw std::runtime_error("ANOMALY_RATE must be between 0.0 and 1.0");
        if (dbHost_.empty())
            throw std::runtime_error("DB_HOST is required");
        if (dbPort_.empty())
            throw std::runtime_error("DB_PORT is required");
        if (dbName_.empty())
            throw std::runtime_error("DB_NAME is required");
        if (dbUser_.empty())
            throw std::runtime_error("DB_USER is required");
        if (dbMaxBufferSize_ == 0)
            throw std::runtime_error("DB_MAX_BUFFER_SIZE must be greater than 0");
        if (mqttBroker_.empty())
            throw std::runtime_error("MQTT_BROKER is required");
        if (mqttClientId_.empty())
            throw std::runtime_error("MQTT_CLIENT_ID is required");
        if (mqttTopic_.empty())
            throw std::runtime_error("MQTT_TOPIC is required");
        if (telegramEnabled_)
        {
            if (telegramBotToken_.empty())
                throw std::runtime_error("TELEGRAM_BOT_TOKEN is required when Telegram is enabled");

            if (telegramChatId_.empty())
                throw std::runtime_error("TELEGRAM_CHAT_ID is required when Telegram is enabled");
        }
    }

    void AppConfig::clearSensitiveData()
    {
        // TODO: на содиум поменять
        dbPassword_.clear();
        telegramBotToken_.clear();

        dbPassword_.shrink_to_fit();
        telegramBotToken_.shrink_to_fit();
    }

} // namespace dorm_energy::application

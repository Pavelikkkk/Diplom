#include "dorm_energy/application/config/app_config.hpp"

#include <algorithm>
#include <chrono>
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
    std::string trim(
        std::string value)
    {
        const auto first = value.find_first_not_of(" \t\r\n");

        if (first == std::string::npos)
            return "";

        const auto last = value.find_last_not_of(" \t\r\n");

        return value.substr(first, last - first + 1);
    }

    std::string strToLower(
        std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return value;
    }

    bool parseBool(
        const std::string &value)
    {
        const std::string normalized = strToLower(trim(value));

        return normalized == "1" || normalized == "true" || normalized == "on" ||
               normalized == "yes";
    }

    int parseIntOrThrow(
        const std::string &key,
        const std::string &value)
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

    double parseDoubleOrThrow(
        const std::string &key,
        const std::string &value)
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

    std::optional<std::size_t> parseSize(
        const std::string &value)
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

    void warnInvalidConfigValue(
        const std::string &key,
        const std::string &value,
        const std::string &fallback)
    {
        std::cerr << "Warning: invalid " << key << " value \"" << value
                  << "\", using default/current value: " << fallback << '\n';
    }

    void applyOptionalSize(
        std::size_t &target,
        const std::string &key,
        const std::string &value)
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

    std::filesystem::path resolveEnvFilePath(
        const std::string &filename)
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

    AppConfig AppConfig::loadFromEnvFile(
        const std::string &filename)
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
                config.generatorConfig_.seed = parseIntOrThrow(key, value);
            else if (key == "INJECT_ANOMALIES")
                config.generatorConfig_.injectAnomalies = parseBool(value);
            else if (key == "ANOMALY_RATE")
            {
                const double anomalyRate = parseDoubleOrThrow(key, value);
                config.generatorConfig_.pointAnomalyRate = anomalyRate;
                config.generatorConfig_.scenarioAnomalyRate = anomalyRate;
            }
            else if (key == "SIMULATION_DATASET_PATH")
                config.simulationDatasetPath_ = value;
            else if (key == "SIMULATION_LABELS_PATH")
                config.simulationLabelsPath_ = value;
            else if (key == "SIMULATION_ANOMALY_REPORT_PATH")
                config.simulationAnomalyReportPath_ = value;
            else if (key == "ONNX_MODEL_PATH")
                config.onnxModelPath_ = value;
            else if (key == "ONNX_ANOMALY_THRESHOLD")
                config.onnxModelConfig_.anomalyThreshold = static_cast<float>(parseDoubleOrThrow(key, value));
            else if (key == "RULE_EXTREME_POWER_KW")
                config.ruleBasedDetectorConfig_.extremePowerKw = parseDoubleOrThrow(key, value);
            else if (key == "RULE_EXTREME_LIGHT_LUX")
                config.ruleBasedDetectorConfig_.extremeLightLux = parseDoubleOrThrow(key, value);
            else if (key == "RULE_SUSTAINED_HIGH_POWER_KW")
                config.ruleBasedDetectorConfig_.sustainedHighPowerKw = parseDoubleOrThrow(key, value);
            else if (key == "RULE_UNATTENDED_POWER_KW")
                config.ruleBasedDetectorConfig_.unattendedPowerKw = parseDoubleOrThrow(key, value);
            else if (key == "RULE_SUDDEN_POWER_SPIKE_KW")
                config.ruleBasedDetectorConfig_.suddenPowerSpikeKw = parseDoubleOrThrow(key, value);
            else if (key == "RULE_REPEATED_SPIKE_DELTA_KW")
                config.ruleBasedDetectorConfig_.repeatedSpikeDeltaKw = parseDoubleOrThrow(key, value);
            else if (key == "RULE_REPEATED_SPIKE_MIN_COUNT")
                config.ruleBasedDetectorConfig_.repeatedSpikeMinCount = parseIntOrThrow(key, value);
            else if (key == "DETECTION_HISTORY_WINDOW_MINUTES")
                config.roomStateAggregatorConfig_.historyWindow = std::chrono::minutes(parseIntOrThrow(key, value));
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
            else if (key == "MQTT_USERNAME")
                config.mqttUsername_ = value;
            else if (key == "MQTT_PASSWORD")
                config.mqttPassword_ = value;
            else if (key == "MQTT_TLS_VERIFY")
                config.mqttTlsVerify_ = parseBool(value);
            else if (key == "TELEGRAM_ENABLED")
                config.telegramEnabled_ = parseBool(value);
            else if (key == "TELEGRAM_BOT_TOKEN")
                config.telegramBotToken_ = value;
            else if (key == "TELEGRAM_CHAT_ID")
                config.telegramChatId_ = value;
            else if (key == "JWT_SECRET")
                config.jwtSecret_ = value;
            else if (key == "JWT_TOKEN_LIFETIME_HOURS")
                config.jwtTokenLifetimeHours_ = parseIntOrThrow(key, value);
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
            generatorConfig_.seed = parseIntOrThrow("RANDOM_SEED", val);

        if (const char *val = std::getenv("INJECT_ANOMALIES"))
            generatorConfig_.injectAnomalies = parseBool(val);
        if (const char *val = std::getenv("ANOMALY_RATE"))
        {
            const double anomalyRate = parseDoubleOrThrow("ANOMALY_RATE", val);
            generatorConfig_.pointAnomalyRate = anomalyRate;
            generatorConfig_.scenarioAnomalyRate = anomalyRate;
        }

        if (const char *val = std::getenv("SIMULATION_DATASET_PATH"))
            simulationDatasetPath_ = val;
        if (const char *val = std::getenv("SIMULATION_LABELS_PATH"))
            simulationLabelsPath_ = val;
        if (const char *val = std::getenv("SIMULATION_ANOMALY_REPORT_PATH"))
            simulationAnomalyReportPath_ = val;
        if (const char *val = std::getenv("ONNX_MODEL_PATH"))
            onnxModelPath_ = val;
        if (const char *val = std::getenv("ONNX_ANOMALY_THRESHOLD"))
            onnxModelConfig_.anomalyThreshold = static_cast<float>(parseDoubleOrThrow("ONNX_ANOMALY_THRESHOLD", val));
        if (const char *val = std::getenv("RULE_EXTREME_POWER_KW"))
            ruleBasedDetectorConfig_.extremePowerKw = parseDoubleOrThrow("RULE_EXTREME_POWER_KW", val);
        if (const char *val = std::getenv("RULE_EXTREME_LIGHT_LUX"))
            ruleBasedDetectorConfig_.extremeLightLux = parseDoubleOrThrow("RULE_EXTREME_LIGHT_LUX", val);
        if (const char *val = std::getenv("RULE_SUSTAINED_HIGH_POWER_KW"))
            ruleBasedDetectorConfig_.sustainedHighPowerKw = parseDoubleOrThrow("RULE_SUSTAINED_HIGH_POWER_KW", val);
        if (const char *val = std::getenv("RULE_UNATTENDED_POWER_KW"))
            ruleBasedDetectorConfig_.unattendedPowerKw = parseDoubleOrThrow("RULE_UNATTENDED_POWER_KW", val);
        if (const char *val = std::getenv("RULE_SUDDEN_POWER_SPIKE_KW"))
            ruleBasedDetectorConfig_.suddenPowerSpikeKw = parseDoubleOrThrow("RULE_SUDDEN_POWER_SPIKE_KW", val);
        if (const char *val = std::getenv("RULE_REPEATED_SPIKE_DELTA_KW"))
            ruleBasedDetectorConfig_.repeatedSpikeDeltaKw = parseDoubleOrThrow("RULE_REPEATED_SPIKE_DELTA_KW", val);
        if (const char *val = std::getenv("RULE_REPEATED_SPIKE_MIN_COUNT"))
            ruleBasedDetectorConfig_.repeatedSpikeMinCount = parseIntOrThrow("RULE_REPEATED_SPIKE_MIN_COUNT", val);
        if (const char *val = std::getenv("DETECTION_HISTORY_WINDOW_MINUTES"))
            roomStateAggregatorConfig_.historyWindow = std::chrono::minutes(parseIntOrThrow("DETECTION_HISTORY_WINDOW_MINUTES", val));

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
        if (const char *val = std::getenv("MQTT_USERNAME"))
            mqttUsername_ = val;
        if (const char *val = std::getenv("MQTT_PASSWORD"))
            mqttPassword_ = val;
        if (const char *val = std::getenv("MQTT_TLS_VERIFY"))
            mqttTlsVerify_ = parseBool(val);

        if (const char *val = std::getenv("TELEGRAM_ENABLED"))
            telegramEnabled_ = parseBool(val);
        if (const char *val = std::getenv("TELEGRAM_BOT_TOKEN"))
            telegramBotToken_ = val;
        if (const char *val = std::getenv("TELEGRAM_CHAT_ID"))
            telegramChatId_ = val;
        if (const char *val = std::getenv("JWT_SECRET"))
            jwtSecret_ = val;
        if (const char *val = std::getenv("JWT_TOKEN_LIFETIME_HOURS"))
            jwtTokenLifetimeHours_ = parseIntOrThrow("JWT_TOKEN_LIFETIME_HOURS", val);
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

    DatabaseConfig AppConfig::getDatabaseConfig() const
    {
        return DatabaseConfig{
            .host = dbHost_,
            .port = dbPort_,
            .name = dbName_,
            .user = dbUser_,
            .password = dbPassword_,
            .maxBufferSize = dbMaxBufferSize_,
        };
    }

    MqttConfig AppConfig::getMqttConfig() const
    {
        return MqttConfig{
            .broker = mqttBroker_,
            .clientId = mqttClientId_,
            .topic = mqttTopic_,
            .username = mqttUsername_,
            .password = mqttPassword_,
            .tlsVerify = mqttTlsVerify_,
        };
    }

    notifier::TelegramConfig AppConfig::getTelegramConfig() const
    {
        return notifier::TelegramConfig{
            .enabled = telegramEnabled_,
            .botToken = telegramBotToken_,
            .chatId = telegramChatId_,
            .maxQueueSize = 500,
        };
    }

    JwtConfig AppConfig::getJwtConfig() const
    {
        return JwtConfig{
            .secret = jwtSecret_,
            .tokenLifetimeHours = jwtTokenLifetimeHours_,
        };
    }

    void AppConfig::validate() const
    {
        if (logLevel_.empty())
            throw std::runtime_error("LOG_LEVEL is not set");
        if (simulationDays_ <= 0)
            throw std::runtime_error("SIMULATION_DAYS must be positive");
        if (generatorConfig_.seed < 0)
            throw std::runtime_error("RANDOM_SEED must be non-negative");
        if (generatorConfig_.pointAnomalyRate < 0.0 || generatorConfig_.pointAnomalyRate > 1.0)
            throw std::runtime_error("ANOMALY_RATE must be between 0.0 and 1.0");
        if (generatorConfig_.scenarioAnomalyRate < 0.0 || generatorConfig_.scenarioAnomalyRate > 1.0)
            throw std::runtime_error("ANOMALY_RATE must be between 0.0 and 1.0");
        if (simulationDatasetPath_.empty())
            throw std::runtime_error("SIMULATION_DATASET_PATH is required");
        if (simulationLabelsPath_.empty())
            throw std::runtime_error("SIMULATION_LABELS_PATH is required");
        if (simulationAnomalyReportPath_.empty())
            throw std::runtime_error("SIMULATION_ANOMALY_REPORT_PATH is required");
        if (onnxModelPath_.empty())
            throw std::runtime_error("ONNX_MODEL_PATH is required");
        if (onnxModelConfig_.anomalyThreshold <= 0.0f)
            throw std::runtime_error("ONNX_ANOMALY_THRESHOLD must be greater than 0");
        if (roomStateAggregatorConfig_.historyWindow <= std::chrono::minutes{0})
            throw std::runtime_error("DETECTION_HISTORY_WINDOW_MINUTES must be greater than 0");
    }

    void AppConfig::validateForCommand(
        cli::CommandType commandType) const
    {
        validate();

        if (commandType != cli::CommandType::Daemon)
        {
            return;
        }

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
        if (jwtSecret_.empty())
            throw std::runtime_error("JWT_SECRET is required");
        if (jwtTokenLifetimeHours_ <= 0)
            throw std::runtime_error("JWT_TOKEN_LIFETIME_HOURS must be greater than 0");
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
        mqttPassword_.clear();
        telegramBotToken_.clear();
        jwtSecret_.clear();

        dbPassword_.shrink_to_fit();
        mqttPassword_.shrink_to_fit();
        telegramBotToken_.shrink_to_fit();
        jwtSecret_.shrink_to_fit();
    }

} // namespace dorm_energy::application

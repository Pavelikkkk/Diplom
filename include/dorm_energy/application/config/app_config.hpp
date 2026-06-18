// include/dorm_energy/application/config/app_config.hpp
#pragma once

#include <cstddef>
#include <string>

namespace dorm_energy::application
{
    class AppConfig // в будущем возможно раздилить его
    {
    public:
        bool isVerbose() const { return verbose_; }
        const std::string &getLogLevel() const { return logLevel_; }

        int getSimulationDays() const { return simulationDays_; }
        int getRandomSeed() const { return randomSeed_; }
        bool getInjectAnomalies() const { return injectAnomalies_; }
        double getAnomalyRate() const { return anomalyRate_; }

        const std::string &getDbHost() const { return dbHost_; }
        const std::string &getDbPort() const { return dbPort_; }
        const std::string &getDbName() const { return dbName_; }
        const std::string &getDbUser() const { return dbUser_; }

        // TODO: заменить на содиум
        const std::string &getDbPassword() const { return dbPassword_; }

        std::size_t getDbMaxBufferSize() const { return dbMaxBufferSize_; }

        const std::string &getMqttBroker() const { return mqttBroker_; }
        const std::string &getMqttClientId() const { return mqttClientId_; }
        const std::string &getMqttTopic() const { return mqttTopic_; }
        const std::string &getMqttUsername() const { return mqttUsername_; }
        const std::string &getMqttPassword() const { return mqttPassword_; }
        bool getMqttTlsVerify() const { return mqttTlsVerify_; }

        bool isTelegramEnabled() const { return telegramEnabled_; }

        // TODO: заменить на содиум
        const std::string &getTelegramBotToken() const { return telegramBotToken_; }

        const std::string &getTelegramChatId() const { return telegramChatId_; }

        static AppConfig load();
        static AppConfig loadFromEnvironment();
        static AppConfig loadFromEnvFile(const std::string &filename = ".env");

        void overrideFromEnvironment();

        std::string getDbConnectionString() const;

        void validate() const;
        void clearSensitiveData();

    private:
        bool verbose_{false};
        std::string logLevel_{"info"};

        int simulationDays_{30};
        int randomSeed_{42};
        bool injectAnomalies_{true};
        double anomalyRate_{0.03};

        std::string dbHost_{};
        std::string dbPort_{"5432"};
        std::string dbName_{};
        std::string dbUser_{};

        // TODO: заменить на содиум
        std::string dbPassword_{};

        std::size_t dbMaxBufferSize_{2000};

        std::string mqttBroker_{};
        std::string mqttClientId_{"dorm-energy"};
        std::string mqttTopic_{"devices/+/power"};
        std::string mqttUsername_{};
        std::string mqttPassword_{};
        bool mqttTlsVerify_{false};

        bool telegramEnabled_{false};

        // TODO: заменить на содиум
        std::string telegramBotToken_{};

        std::string telegramChatId_{};
    };

} // namespace dorm_energy::application

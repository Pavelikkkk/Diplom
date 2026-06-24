// include/dorm_energy/application/config/app_config.hpp
#pragma once

#include "dorm_energy/application/cli/command_type.hpp"
#include "dorm_energy/application/config/database_config.hpp"
#include "dorm_energy/application/config/generator_config.hpp"
#include "dorm_energy/application/config/jwt_config.hpp"
#include "dorm_energy/application/config/mqtt_config.hpp"
#include "dorm_energy/application/config/onnx_model_config.hpp"
#include "dorm_energy/application/config/rule_based_detector_config.hpp"
#include "dorm_energy/application/config/room_state_aggregator_config.hpp"
#include "dorm_energy/infrastructure/notifier/telegram_config.hpp"

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
        int getRandomSeed() const { return generatorConfig_.seed; }
        bool getInjectAnomalies() const { return generatorConfig_.injectAnomalies; }
        double getAnomalyRate() const { return generatorConfig_.pointAnomalyRate; }
        const std::string &getSimulationDatasetPath() const { return simulationDatasetPath_; }
        const std::string &getSimulationLabelsPath() const { return simulationLabelsPath_; }
        const std::string &getSimulationAnomalyReportPath() const { return simulationAnomalyReportPath_; }
        const simulation::SyntheticDataGeneratorConfig &getGeneratorConfig() const { return generatorConfig_; }

        const std::string &getOnnxModelPath() const { return onnxModelPath_; }
        const detection::OnnxModelConfig &getOnnxModelConfig() const { return onnxModelConfig_; }
        const detection::RuleBasedDetectorConfig &getRuleBasedDetectorConfig() const { return ruleBasedDetectorConfig_; }
        const detection::RoomStateAggregatorConfig &getRoomStateAggregatorConfig() const { return roomStateAggregatorConfig_; }

        const std::string &getDbHost() const { return dbHost_; }
        const std::string &getDbPort() const { return dbPort_; }
        const std::string &getDbName() const { return dbName_; }
        const std::string &getDbUser() const { return dbUser_; }

        // TODO: заменить на содиум
        const std::string &getDbPassword() const { return dbPassword_; }

        std::size_t getDbMaxBufferSize() const { return dbMaxBufferSize_; }
        DatabaseConfig getDatabaseConfig() const;

        const std::string &getMqttBroker() const { return mqttBroker_; }
        const std::string &getMqttClientId() const { return mqttClientId_; }
        const std::string &getMqttTopic() const { return mqttTopic_; }
        const std::string &getMqttUsername() const { return mqttUsername_; }
        const std::string &getMqttPassword() const { return mqttPassword_; }
        bool getMqttTlsVerify() const { return mqttTlsVerify_; }
        MqttConfig getMqttConfig() const;

        bool isTelegramEnabled() const { return telegramEnabled_; }

        // TODO: заменить на содиум
        const std::string &getTelegramBotToken() const { return telegramBotToken_; }

        const std::string &getTelegramChatId() const { return telegramChatId_; }
        notifier::TelegramConfig getTelegramConfig() const;

        const std::string &getJwtSecret() const { return jwtSecret_; }
        int getJwtTokenLifetimeHours() const { return jwtTokenLifetimeHours_; }
        JwtConfig getJwtConfig() const;

        static AppConfig load();
        static AppConfig loadFromEnvironment();
        static AppConfig loadFromEnvFile(
            const std::string &filename = ".env");

        void overrideFromEnvironment();

        std::string getDbConnectionString() const;

        void validate() const;
        void validateForCommand(
            cli::CommandType commandType) const;
            
        void clearSensitiveData();

    private:
        bool verbose_{false}; // -
        std::string logLevel_{"info"};

        int simulationDays_{60};
        simulation::SyntheticDataGeneratorConfig generatorConfig_{};
        
        std::string simulationDatasetPath_{"../../data/training_dataset.csv"};
        std::string simulationLabelsPath_{"../../data/labels.csv"};
        std::string simulationAnomalyReportPath_{"../../data/anomaly_report.csv"};

        std::string onnxModelPath_{"../../ml/models/anomaly_autoencoder.onnx"};
        detection::OnnxModelConfig onnxModelConfig_{};
        detection::RuleBasedDetectorConfig ruleBasedDetectorConfig_{};
        detection::RoomStateAggregatorConfig roomStateAggregatorConfig_{};

        std::string dbHost_{};
        std::string dbPort_{"5432"};
        std::string dbName_{};
        std::string dbUser_{};

        // TODO: заменить на содиум
        std::string dbPassword_{};

        std::size_t dbMaxBufferSize_{2000};

        std::string mqttBroker_{};
        std::string mqttClientId_{"dorm-energy"};
        std::string mqttTopic_{"devices/+/+"};
        std::string mqttUsername_{};
        std::string mqttPassword_{};
        bool mqttTlsVerify_{false};

        bool telegramEnabled_{false};

        // TODO: заменить на содиум
        std::string telegramBotToken_{};

        std::string telegramChatId_{};

        std::string jwtSecret_{};
        int jwtTokenLifetimeHours_{168};
    };

} // namespace dorm_energy::application

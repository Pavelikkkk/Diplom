// include/dorm_energy/domain/mqtt/imqtt_connection.hpp
#pragma once

#include <string>

namespace dorm_energy::mqtt
{

    /**
     * @brief Режим работы MQTT-клиента
     */
    enum class MqttMode
    {
        Simulation, ///< Симуляция (генерация данных локально)
        Real        ///< Реальное подключение к брокеру
    };

    /**
     * @brief Интерфейс для управления подключением и жизненным циклом MQTT
     */
    class IMqttConnection
    {
    public:
        virtual ~IMqttConnection() = default;

        /**
         * @brief Подключается к MQTT-брокеру
         * @param broker   адрес брокера
         * @param client_id идентификатор клиента
         */
        virtual bool connect(const std::string &broker = "tcp://127.0.0.1:1883",
                             const std::string &client_id = "dorm-energy",
                             const std::string &username = "",
                             const std::string &password = "",
                             bool tlsVerify = false) = 0;

        virtual bool isConnected() const = 0;

        virtual bool start() = 0;
        virtual void stop() = 0;

        /**
         * @brief Переключает режим работы (симуляция / реальный)
         */
        virtual void setMode(MqttMode mode) = 0;
    };

} // namespace dorm_energy::mqtt

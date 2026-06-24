#include "dorm_energy/infrastructure/mqtt/mqtt_client.hpp"

#include "dorm_energy/infrastructure/mqtt/message_parser.hpp"

#include <chrono>
#include <mqtt/async_client.h>
#include <mutex>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <utility>
#include <vector>

namespace dorm_energy::mqtt
{
    namespace
    {
        constexpr int Qos = 1; // в кониг

        bool usesTls(
            const std::string &broker)
        {
            return broker.rfind("ssl://", 0) == 0 || broker.rfind("wss://", 0) == 0;
        }
    }

    class MqttClient::Impl
    {
    public:
        Impl();

        ~Impl();

        bool connect(
            const std::string &broker,
            const std::string &clientId,
            const std::string &username,
            const std::string &password,
            bool tlsVerify);

        bool isConnected() const;

        bool start();

        void stop();

        void subscribe(
            const std::string &topic);

        // void subscribe(
        //     const std::vector<std::string> &topics);

        void unsubscribe(
            const std::string &topic);

        void setHandler(
            std::unique_ptr<application::IMessageHandler> handler);

    private:
        struct CallbackHandler final : public ::mqtt::callback
        {
            explicit CallbackHandler(
                Impl &parent);

            void connected(
                const std::string &cause) override;

            void connection_lost(
                const std::string &cause) override;

            void message_arrived(
                ::mqtt::const_message_ptr message) override;

            Impl &parent_;
        };

    private:
        ::mqtt::connect_options makeConnectOptions(
            const std::string &broker,
            const std::string &username,
            const std::string &password,
            bool tlsVerify) const;

        void handleIncomingPayload(
            const std::string &payload,
            const std::string &topic);

    private:
        std::unique_ptr<::mqtt::async_client> client_;

        ::mqtt::connect_options connectionOptions_;

        std::unique_ptr<CallbackHandler> callbackHandler_;

        std::unique_ptr<application::IMessageHandler> handler_;

        MessageParser parser_;

        std::atomic<bool> connected_{false};

        std::atomic<bool> running_{false};

        mutable std::mutex mutex_;
    };

    MqttClient::Impl::CallbackHandler::CallbackHandler(
        Impl &parent)
        : parent_(parent)
    {
    }

    void MqttClient::Impl::CallbackHandler::connected(
        const std::string &)
    {
        parent_.connected_ = true;
    }

    void MqttClient::Impl::CallbackHandler::connection_lost(
        const std::string &)
    {
        parent_.connected_ = false;
    }

    void MqttClient::Impl::CallbackHandler::message_arrived(
        ::mqtt::const_message_ptr message)
    {
        if (!message)
        {
            return;
        }

        parent_.handleIncomingPayload(message->to_string(), message->get_topic());
    }

    MqttClient::Impl::Impl()
        : callbackHandler_(
              std::make_unique<CallbackHandler>(*this))
    {
    }

    MqttClient::Impl::~Impl()
    {
        stop();
    }

    bool MqttClient::Impl::connect(
        const std::string &broker,
        const std::string &clientId,
        const std::string &username,
        const std::string &password,
        bool tlsVerify)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (broker.empty())
        {
            throw std::invalid_argument("MQTT broker must not be empty");
        }

        if (clientId.empty())
        {
            throw std::invalid_argument("MQTT clientId must not be empty");
        }

        client_ = std::make_unique<::mqtt::async_client>(
            broker,
            clientId);

        client_->set_callback(*callbackHandler_);

        connectionOptions_ = makeConnectOptions(
            broker,
            username,
            password,
            tlsVerify);

        try
        {
            auto token = client_->connect(connectionOptions_);

            token->wait_for(std::chrono::seconds{10});

            connected_ = true;

            return true;
        }
        catch (const ::mqtt::exception &)
        {
            connected_ = false;

            return false;
        }
    }

    bool MqttClient::Impl::isConnected() const
    {
        return connected_;
    }

    bool MqttClient::Impl::start()
    {
        running_ = true;

        return true;
    }

    void MqttClient::Impl::stop()
    {
        running_ = false;

        if (handler_)
        {
            handler_->flush();
        }

        std::lock_guard<std::mutex> lock(mutex_);

        if (!client_ || !connected_)
        {
            connected_ = false;
            return;
        }

        try
        {
            client_->disconnect()->wait();
        }
        catch (const ::mqtt::exception &)
        {
        }

        connected_ = false;
    }

    void MqttClient::Impl::subscribe(
        const std::string &topic)
    {
        if (topic.empty())
        {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);

        if (!client_ || !connected_)
        {
            return;
        }

        try
        {
            client_->subscribe(topic, Qos)->wait();
        }
        catch (const ::mqtt::exception &)
        {
        }
    }

    void MqttClient::Impl::unsubscribe(
        const std::string &topic)
    {
        if (topic.empty())
        {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);

        if (!client_ || !connected_)
        {
            return;
        }

        try
        {
            client_->unsubscribe(topic)->wait();
        }
        catch (const ::mqtt::exception &)
        {
        }
    }

    void MqttClient::Impl::setHandler(
        std::unique_ptr<application::IMessageHandler> handler)
    {
        handler_ = std::move(handler);
    }

    ::mqtt::connect_options MqttClient::Impl::makeConnectOptions(
        const std::string &broker,
        const std::string &username,
        const std::string &password,
        bool tlsVerify) const
    {
        ::mqtt::connect_options options;

        options.set_keep_alive_interval(20);
        options.set_clean_session(true);
        options.set_automatic_reconnect(true);
        options.set_connect_timeout(10);

        if (!username.empty())
        {
            options.set_user_name(username);
            options.set_password(password);
        }

        if (usesTls(broker))
        {
            ::mqtt::ssl_options sslOptions;
            sslOptions.set_enable_server_cert_auth(tlsVerify); // только для тестов

            options.set_ssl(sslOptions);
        }

        return options;
    }

    void MqttClient::Impl::handleIncomingPayload(
        const std::string &payload,
        const std::string &topic)
    {
        if (!running_)
        {
            spdlog::debug("MQTT payload ignored because client is not running");
            return;
        }

        if (!handler_)
        {
            spdlog::warn("MQTT payload ignored because message handler is not configured");
            return;
        }

        const auto reading = parser_.parse(payload);

        if (!reading.has_value())
        {
            spdlog::warn("MQTT payload ignored because it could not be parsed: topic={} bytes={} payload={}",
                         topic, payload.size(), payload.empty() ? "<empty>" : payload);
            return;
        }

        spdlog::info("[MessageParser] Parsed: {} | {} | {:.2f} {}",
                     reading->deviceId, reading->sensorType, reading->value, reading->unit);

        handler_->handle(reading.value());
    }

    MqttClient::MqttClient()
        : pimpl_(std::make_unique<Impl>())
    {
    }

    MqttClient::~MqttClient() = default;

    bool MqttClient::connect(
        const std::string &broker,
        const std::string &clientId,
        const std::string &username,
        const std::string &password,
        bool tlsVerify)
    {
        return pimpl_->connect(
            broker,
            clientId,
            username,
            password,
            tlsVerify);
    }

    bool MqttClient::isConnected() const
    {
        return pimpl_->isConnected();
    }

    bool MqttClient::start()
    {
        return pimpl_->start();
    }

    void MqttClient::stop()
    {
        pimpl_->stop();
    }

    void MqttClient::subscribe(
        const std::string &topic)
    {
        pimpl_->subscribe(topic);
    }

    void MqttClient::unsubscribe(
        const std::string &topic)
    {
        pimpl_->unsubscribe(topic);
    }

    void MqttClient::setHandler(
        std::unique_ptr<application::IMessageHandler> handler)
    {
        pimpl_->setHandler(std::move(handler));
    }
} // namespace dorm_energy::mqtt

#include "dorm_energy/infrastructure/mqtt/mqtt_client.hpp"
#include "dorm_energy/infrastructure/mqtt/message_parser.hpp"

#include <mqtt/async_client.h>
#include <fmt/format.h>
#include <thread>
#include <chrono>

namespace dorm_energy::mqtt
{
    namespace
    {
        bool usesTls(const std::string &broker)
        {
            return broker.rfind("ssl://", 0) == 0 || broker.rfind("wss://", 0) == 0;
        }
    }

    class MqttClient::Impl
    {
    public:
        Impl();
        ~Impl();

        bool connect(const std::string &broker, const std::string &clientId,
                     const std::string &username, const std::string &password,
                     bool tlsVerify);
        bool isConnected() const;
        bool start();
        void stop();
        void setMode(MqttMode mode);

        void subscribe(const std::string &topic);
        void subscribe(const std::vector<std::string> &topics);
        void unsubscribe(const std::string &topic);

        void setHandler(std::unique_ptr<application::IMessageHandler> handler);
        void setMessageCallback(MessageCallback callback);

    private:
        struct CallbackHandler final : public ::mqtt::callback
        {
            explicit CallbackHandler(Impl &parent) : parent_(parent) {}

            void connected(const std::string &cause) override;
            void connection_lost(const std::string &cause) override;
            void message_arrived(::mqtt::const_message_ptr msg) override;

            Impl &parent_;
        };

        MqttMode currentMode_ = MqttMode::Simulation;
        std::unique_ptr<::mqtt::async_client> client_;
        ::mqtt::connect_options connOpts_;

        std::string broker_;
        std::string clientId_;
        std::string username_;
        std::string password_;
        bool tlsVerify_{false};

        std::atomic<bool> connected_{false};
        std::atomic<bool> running_{false};

        std::unique_ptr<application::IMessageHandler> handler_;
        MessageCallback messageCallback_ = nullptr;

        MessageParser parser_;

        std::unique_ptr<CallbackHandler> callbackHandler_;
        std::mutex mutex_;

        std::atomic<int> reconnectAttempts_{0};
        static constexpr int MAX_RECONNECT_ATTEMPTS = 5;
    };

    MqttClient::Impl::Impl()
    {
        callbackHandler_ = std::make_unique<CallbackHandler>(*this);
    }

    MqttClient::Impl::~Impl()
    {
        stop();
    }

    bool MqttClient::Impl::connect(const std::string &broker, const std::string &clientId,
                                   const std::string &username, const std::string &password,
                                   bool tlsVerify)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        broker_ = broker;
        clientId_ = clientId;
        username_ = username;
        password_ = password;
        tlsVerify_ = tlsVerify;

        if (currentMode_ == MqttMode::Simulation)
        {
            std::cout << "[MQTT] SIMULATION MODE activated\n";
            connected_ = true;
            reconnectAttempts_ = 0;
            return true;
        }

        client_ = std::make_unique<::mqtt::async_client>(broker, clientId);
        client_->set_callback(*callbackHandler_);

        connOpts_ = ::mqtt::connect_options{};
        connOpts_.set_keep_alive_interval(20);
        connOpts_.set_clean_session(true);
        connOpts_.set_automatic_reconnect(true);
        connOpts_.set_connect_timeout(10);
        if (usesTls(broker_))
        {
            ::mqtt::ssl_options sslOpts;
            sslOpts.set_enable_server_cert_auth(tlsVerify_);
            connOpts_.set_ssl(sslOpts);
        }
        if (!username_.empty())
        {
            connOpts_.set_user_name(username_);
            connOpts_.set_password(password_);
        }

        try
        {
            std::cout << "[MQTT] Connecting to " << broker << "...\n";
            auto token = client_->connect(connOpts_);
            token->wait_for(std::chrono::seconds(10));

            connected_ = true;
            reconnectAttempts_ = 0;
            return true;
        }
        catch (const ::mqtt::exception &e)
        {
            std::cerr << "[MQTT] Connection failed: " << e.what() << std::endl;
            return false;
        }
    }

    bool MqttClient::Impl::isConnected() const { return connected_; }

    bool MqttClient::Impl::start()
    {
        running_ = true;
        std::cout << "[MQTT] Started in "
                  << (currentMode_ == MqttMode::Simulation ? "SIMULATION" : "REAL")
                  << " mode\n";
        return true;
    }

    void MqttClient::Impl::stop()
    {
        running_ = false;
        if (client_ && connected_)
        {
            try
            {
                std::cout << "[MQTT] Graceful disconnecting...\n";
                client_->disconnect()->wait();
            }
            catch (...)
            {
            }
        }
        connected_ = false;
        std::cout << "[MQTT] Stopped\n";
    }

    void MqttClient::Impl::setMode(MqttMode mode)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMode_ = mode;
        std::cout << "[MqttClient] Mode changed to: " << (mode == MqttMode::Simulation ? "SIMULATION" : "REAL") << std::endl;
    }

    void MqttClient::Impl::subscribe(const std::string &topic)
    {
        if (currentMode_ == MqttMode::Simulation)
        {
            std::cout << "[MQTT][SIM] Subscribed to: " << topic << std::endl;
            return;
        }
        if (client_ && connected_)
            client_->subscribe(topic, 1);
    }

    void MqttClient::Impl::subscribe(const std::vector<std::string> &topics)
    {
        for (const auto &t : topics)
            subscribe(t);
    }

    void MqttClient::Impl::unsubscribe(const std::string &topic)
    {
        if (client_ && connected_)
            client_->unsubscribe(topic);
    }

    void MqttClient::Impl::setHandler(std::unique_ptr<application::IMessageHandler> handler)
    {
        handler_ = std::move(handler);
    }

    void MqttClient::Impl::setMessageCallback(MessageCallback callback)
    {
        messageCallback_ = std::move(callback);
    }

    void MqttClient::Impl::CallbackHandler::connected(const std::string &cause)
    {
        std::cout << "[MQTT] Connected: " << cause << std::endl;
        parent_.connected_ = true;
        parent_.reconnectAttempts_ = 0;
    }

    void MqttClient::Impl::CallbackHandler::connection_lost(const std::string &cause)
    {
        std::cerr << "[MQTT] Connection lost: " << cause << std::endl;
        parent_.connected_ = false;

        if (parent_.reconnectAttempts_ < Impl::MAX_RECONNECT_ATTEMPTS)
        {
            parent_.reconnectAttempts_++;
            std::cout << "[MQTT] Scheduling reconnect attempt " << parent_.reconnectAttempts_ << "/" << Impl::MAX_RECONNECT_ATTEMPTS << std::endl;

            std::thread([this]()
                        {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            parent_.connect(parent_.broker_, parent_.clientId_, parent_.username_, parent_.password_, parent_.tlsVerify_); }).detach();
        }
    }

    void MqttClient::Impl::CallbackHandler::message_arrived(::mqtt::const_message_ptr msg)
    {
        try
        {
            auto reading_opt = parent_.parser_.parse(msg->to_string());
            if (reading_opt)
            {
                if (parent_.handler_)
                    parent_.handler_->handle(*reading_opt);

                if (parent_.messageCallback_)
                    parent_.messageCallback_(*reading_opt);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "[MQTT] Message processing error: " << e.what() << std::endl;
        }
    }

    MqttClient::MqttClient() : pimpl_(std::make_unique<Impl>()) {}
    MqttClient::~MqttClient() = default;

    bool MqttClient::connect(const std::string &broker, const std::string &clientId,
                             const std::string &username, const std::string &password,
                             bool tlsVerify)
    {
        return pimpl_->connect(broker, clientId, username, password, tlsVerify);
    }

    bool MqttClient::isConnected() const { return pimpl_->isConnected(); }
    bool MqttClient::start() { return pimpl_->start(); }
    void MqttClient::stop() { pimpl_->stop(); }
    void MqttClient::setMode(MqttMode mode) { pimpl_->setMode(mode); }

    void MqttClient::subscribe(const std::string &topic) { pimpl_->subscribe(topic); }
    void MqttClient::subscribe(const std::vector<std::string> &topics) { pimpl_->subscribe(topics); }
    void MqttClient::unsubscribe(const std::string &topic) { pimpl_->unsubscribe(topic); }

    void MqttClient::setHandler(std::unique_ptr<application::IMessageHandler> handler)
    {
        pimpl_->setHandler(std::move(handler));
    }

    void MqttClient::setMessageCallback(MessageCallback callback)
    {
        pimpl_->setMessageCallback(std::move(callback));
    }

} // namespace dorm_energy::mqtt

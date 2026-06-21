# ============================================
# РћСЃРЅРѕРІРЅР°СЏ Р±РёР±Р»РёРѕС‚РµРєР° РїСЂРѕРµРєС‚Р°
# ============================================

add_library(dorm_energy_core STATIC
    src/dorm_energy/core/alert_severity.cpp
    src/dorm_energy/core/time_utils.cpp
    src/dorm_energy/core/csv_utils.cpp
    src/dorm_energy/domain/logging/log_level.cpp

    # Infrastructure
    src/dorm_energy/infrastructure/logging/spdlog_logger.cpp
    src/dorm_energy/infrastructure/simulation/synthetic_data_generator.cpp
    src/dorm_energy/infrastructure/notifier/telegram_notifier.cpp
    src/dorm_energy/infrastructure/handlers/message_handler.cpp
    src/dorm_energy/infrastructure/cli/cli_parser.cpp
    src/dorm_energy/infrastructure/mqtt/mqtt_client.cpp
    src/dorm_energy/infrastructure/mqtt/message_parser.cpp
    src/dorm_energy/infrastructure/storage/postgres_repository.cpp
    src/dorm_energy/infrastructure/storage/postgres_repository_admin.cpp
    src/dorm_energy/infrastructure/storage/postgres_repository_anomalies.cpp
    src/dorm_energy/infrastructure/storage/postgres_repository_analytics.cpp
    src/dorm_energy/infrastructure/storage/postgres_repository_catalog.cpp
    src/dorm_energy/infrastructure/storage/postgres_repository_users.cpp
    src/dorm_energy/infrastructure/detection/rule_based_detector.cpp
    src/dorm_energy/infrastructure/notifier/telegram_queue.cpp
    src/dorm_energy/infrastructure/simulation/csv_exporter.cpp
    src/dorm_energy/infrastructure/simulation/anomaly_report_exporter.cpp
    src/dorm_energy/infrastructure/simulation/label_exporter.cpp
    src/dorm_energy/infrastructure/detection/room_state_aggregator.cpp
    src/dorm_energy/infrastructure/detection/anomaly_tracker.cpp
    src/dorm_energy/infrastructure/detection/onnx_detector.cpp
    src/dorm_energy/infrastructure/detection/hybrid_detector.cpp
    src/dorm_energy/infrastructure/web/controllers/admin_controller.cpp
    src/dorm_energy/infrastructure/web/controllers/account_controller.cpp
    src/dorm_energy/infrastructure/web/controllers/analytics_controller.cpp
    src/dorm_energy/infrastructure/web/controllers/anomalies_controller.cpp
    src/dorm_energy/infrastructure/web/controllers/auth_controller.cpp
    src/dorm_energy/infrastructure/web/controllers/health_controller.cpp
    src/dorm_energy/infrastructure/web/controllers/resources_controller.cpp
    src/dorm_energy/infrastructure/web/middleware/auth_middleware.cpp
    src/dorm_energy/infrastructure/web/middleware/cors_middleware.cpp
    src/dorm_energy/infrastructure/web/routes/route_registrar.cpp
    src/dorm_energy/infrastructure/web/server/web_server.cpp
    src/dorm_energy/infrastructure/web/utils/dto_json_mapper.cpp
    src/dorm_energy/infrastructure/web/utils/json_response.cpp
    src/dorm_energy/infrastructure/auth/jwt_service.cpp
    src/dorm_energy/infrastructure/auth/bcrypt_password_hasher.cpp

    # Application
    src/dorm_energy/application/config/app_config.cpp
    src/dorm_energy/application/factories/auth_factory.cpp
    src/dorm_energy/application/factories/cli_factory.cpp
    src/dorm_energy/application/factories/command_factory.cpp
    src/dorm_energy/application/factories/detection_factory.cpp
    src/dorm_energy/application/factories/logger_factory.cpp
    src/dorm_energy/application/factories/message_handler_factory.cpp
    src/dorm_energy/application/factories/mqtt_factory.cpp
    src/dorm_energy/application/factories/notification_factory.cpp
    src/dorm_energy/application/factories/repository_factory.cpp
    src/dorm_energy/application/factories/simulation_factory.cpp
    src/dorm_energy/application/factories/state_factory.cpp
    src/dorm_energy/application/factories/web_server_factory.cpp
    src/dorm_energy/application/commands/simulate_command.cpp
    src/dorm_energy/application/commands/daemon_command.cpp
    src/dorm_energy/application/application_builder.cpp
    src/dorm_energy/application/application.cpp
    src/dorm_energy/application/runtime.cpp
    src/dorm_energy/application/notification/notification_message_factory.cpp
    src/dorm_energy/application/notification/notifier_service.cpp
    src/dorm_energy/application/auth/auth_service.cpp
)

target_include_directories(
    dorm_energy_core
    PRIVATE
        ${ONNXRUNTIME_INCLUDE_DIR}
)

target_link_libraries(dorm_energy_core
    PUBLIC
        CLI11::CLI11
        spdlog::spdlog
        fmt::fmt
        nlohmann_json::nlohmann_json
        libpqxx::pqxx
        OpenSSL::SSL
        OpenSSL::Crypto
        PahoMqttCpp::paho-mqttpp3
        CURL::libcurl
        Drogon::Drogon
        jwt-cpp::jwt-cpp
        ${BCRYPT_LIBRARY}
        ${ONNXRUNTIME_LIB}
)

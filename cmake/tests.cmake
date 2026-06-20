# ============================================
# Unit tests
# ============================================

enable_testing()

include(GoogleTest)

add_executable(dorm_energy_unit_tests
    tests/unit/alert_severity_tests.cpp
    tests/unit/message_parser_tests.cpp
    tests/unit/detection_tests.cpp
    tests/unit/notifier_service_tests.cpp
    tests/unit/auth_service_tests.cpp
    tests/unit/auth_infrastructure_tests.cpp
    tests/integration/postgres_repository_tests.cpp
)

target_link_libraries(dorm_energy_unit_tests
    PRIVATE
        dorm_energy_core
        GTest::gtest
        GTest::gtest_main
)

gtest_discover_tests(dorm_energy_unit_tests)

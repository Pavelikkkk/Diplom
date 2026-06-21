# ============================================
# Зависимости (vcpkg)
# ============================================

find_package(CLI11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PahoMqttCpp CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(jwt-cpp CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_library(BCRYPT_LIBRARY bcrypt REQUIRED)

set(ONNXRUNTIME_ROOT
    ${PROJECT_SOURCE_DIR}/third_party/onnxruntime)

set(ONNXRUNTIME_INCLUDE_DIR
    ${ONNXRUNTIME_ROOT}/include)

set(ONNXRUNTIME_LIB
    ${ONNXRUNTIME_ROOT}/lib/onnxruntime.lib)

message(STATUS "ONNX LIB = ${ONNXRUNTIME_LIB}")
message(STATUS "ONNX INCLUDE = ${ONNXRUNTIME_INCLUDE_DIR}")

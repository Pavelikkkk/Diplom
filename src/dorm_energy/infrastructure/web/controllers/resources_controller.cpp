#include "dorm_energy/infrastructure/web/controllers/resources_controller.hpp"

#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

namespace dorm_energy::web
{
    namespace
    {
        int asIntOrDefault(
            const Json::Value &json,
            const char *key,
            int fallback)
        {
            return json.isMember(key) ? json[key].asInt() : fallback;
        }

        double asDoubleOrDefault(
            const Json::Value &json,
            const char *key,
            double fallback)
        {
            return json.isMember(key) ? json[key].asDouble() : fallback;
        }

        bool asBoolOrDefault(
            const Json::Value &json,
            const char *key,
            bool fallback)
        {
            return json.isMember(key) ? json[key].asBool() : fallback;
        }

        std::string asStringOrDefault(
            const Json::Value &json,
            const char *key,
            const std::string &fallback)
        {
            if (!json.isMember(key))
            {
                return fallback;
            }

            auto value = json[key].asString();
            return value.empty() ? fallback : value;
        }

        std::string makeDeviceId(
            const std::string &value,
            int roomId)
        {
            std::string result;

            for (const auto ch : value)
            {
                if (std::isalnum(static_cast<unsigned char>(ch)))
                {
                    result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
                }
                else if (!result.empty() && result.back() != '-')
                {
                    result.push_back('-');
                }
            }

            while (!result.empty() && result.back() == '-')
            {
                result.pop_back();
            }

            return result.empty() ? "room-" + std::to_string(roomId) : result;
        }

        void requireBusinessAccess(
            const UserDto &user)
        {
            if (user.accountType != "BUSINESS" && user.role != "ADMIN")
            {
                throw std::runtime_error("Business subscription is required");
            }
        }
    }

    void registerResourceRoutes(const WebContext &context)
    {
        auto catalogRepository = context.catalogRepository;
        auto anomalyRepository = context.anomalyRepository;
        auto subscriptionRepository = context.subscriptionRepository;
        auto auth = AuthMiddleware(context.userRepository, context.authService);

        drogon::app().registerHandler(
            "/api/stats",
            [catalogRepository, anomalyRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto user = auth.requireUser(req);

                    json["buildings"] =
                        static_cast<int>(catalogRepository->getBuildings(user.organizationId).size());
                    json["rooms"] =
                        static_cast<int>(catalogRepository->getRooms(user.organizationId).size());
                    json["devices"] =
                        static_cast<int>(catalogRepository->getDevices(user.organizationId).size());
                    json["anomalies"] = static_cast<int>(
                        anomalyRepository->getLatestAnomalies(1000, user.organizationId).size());
                    json["mqttOnline"] = true;
                    json["success"] = true;
                }
                catch (const std::exception &ex)
                {
                    json = makeErrorJson(ex);
                    status = statusForError(ex.what());
                }

                callback(makeJsonResponse(json, status));
            });

        drogon::app().registerHandler(
            "/api/buildings",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto list = catalogRepository->getBuildings(user.organizationId);

                    callback(makeJsonResponse(toJsonArray(list)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/buildings",
            [catalogRepository, subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                                                              std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto user = auth.requireUser(req);
                    requireBusinessAccess(user);

                    auto body = req->getJsonObject();

                    if (!body)
                    {
                        throw std::runtime_error("Invalid JSON body");
                    }

                    const auto subscription = subscriptionRepository->getUserSubscription(user.id);
                    const auto buildings = catalogRepository->getBuildings(user.organizationId);

                    if (subscription.maxBuildings > 0 && static_cast<int>(buildings.size()) >= subscription.maxBuildings)
                    {
                        throw std::runtime_error("Building limit reached");
                    }

                    const auto name = asStringOrDefault(*body, "name", "New Building");
                    const auto address = asStringOrDefault(*body, "address", "");
                    const auto description = asStringOrDefault(*body, "description", "");

                    json["success"] = true;
                    json["id"] = catalogRepository->createBuildingForOrganization(
                        user.organizationId,
                        name,
                        address,
                        description);
                }
                catch (const std::exception &ex)
                {
                    json = makeErrorJson(ex);
                    status = statusForError(ex.what());
                }

                callback(makeJsonResponse(json, status));
            },
            {drogon::Post});

        drogon::app().registerHandler(
            "/api/rooms",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto list = catalogRepository->getRooms(user.organizationId);

                    callback(makeJsonResponse(toJsonArray(list)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/devices",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto list = catalogRepository->getDevices(user.organizationId);

                    callback(makeJsonResponse(toJsonArray(list)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/rooms",
            [catalogRepository, subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                                                              std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto user = auth.requireUser(req);
                    auto body = req->getJsonObject();

                    if (!body)
                    {
                        throw std::runtime_error("Invalid JSON body");
                    }

                    const auto subscription = subscriptionRepository->getUserSubscription(user.id);
                    const auto rooms = catalogRepository->getRooms(user.organizationId);

                    if (subscription.maxRooms > 0 && static_cast<int>(rooms.size()) >= subscription.maxRooms)
                    {
                        throw std::runtime_error("Room limit reached");
                    }

                    const auto buildings = catalogRepository->getBuildings(user.organizationId);
                    const int buildingId = asIntOrDefault(
                        *body,
                        "buildingId",
                        buildings.empty()
                            ? catalogRepository->createBuildingForOrganization(user.organizationId, "Default Building", "", "Created from room setup")
                            : buildings.front().id);

                    const auto roomName = asStringOrDefault(*body, "roomName", "New Room");
                    const auto roomType = asStringOrDefault(*body, "roomType", "General");
                    const int floorNumber = asIntOrDefault(*body, "floorNumber", 0);
                    const double minNormalPowerKw = 0.0;
                    const double maxNormalPowerKw = 2.8;
                    const bool allowUnattendedPower = false;

                    if (maxNormalPowerKw <= minNormalPowerKw)
                    {
                        throw std::runtime_error("maxNormalPowerKw must be greater than minNormalPowerKw");
                    }

                    const int id = catalogRepository->createRoomForBuilding(
                        buildingId,
                        roomName,
                        roomType,
                        floorNumber,
                        minNormalPowerKw,
                        maxNormalPowerKw,
                        allowUnattendedPower);

                    json["success"] = true;
                    json["id"] = id;
                }
                catch (const std::exception &ex)
                {
                    json = makeErrorJson(ex);
                    status = statusForError(ex.what());
                }

                callback(makeJsonResponse(json, status));
            },
            {drogon::Post});

    }
} // namespace dorm_energy::web

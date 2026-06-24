#include "dorm_energy/infrastructure/web/controllers/admin_controller.hpp"

#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

#include <stdexcept>

namespace dorm_energy::web
{
    namespace
    {
        Json::Value requireJsonBody(
            const drogon::HttpRequestPtr &req)
        {
            auto body = req->getJsonObject();

            if (!body)
            {
                throw std::runtime_error("Invalid JSON body");
            }

            return *body;
        }

        int intOrDefault(
            const Json::Value &body,
            const char *key,
            int fallback)
        {
            return body.isMember(key) ? body[key].asInt() : fallback;
        }

        double doubleOrDefault(
            const Json::Value &body,
            const char *key,
            double fallback)
        {
            return body.isMember(key) ? body[key].asDouble() : fallback;
        }

        bool boolOrDefault(
            const Json::Value &body,
            const char *key,
            bool fallback)
        {
            return body.isMember(key) ? body[key].asBool() : fallback;
        }
    }

    void registerAdminRoutes(const WebContext &context)
    {
        auto adminRepository = context.adminRepository;
        auto catalogRepository = context.catalogRepository;
        auto auth = AuthMiddleware(context.userRepository, context.authService);

        drogon::app().registerHandler(
            "/api/admin/overview",
            [adminRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);
                    json = adminRepository->getAdminOverview();
                    json["success"] = true;
                }
                catch (const std::exception &ex)
                {
                    json = makeErrorJson(ex);
                    status = statusForError(ex.what());
                }

                callback(makeJsonResponse(json, status));
            },
            {drogon::Get});

        drogon::app().registerHandler(
            "/api/admin/buildings",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);

                    const auto body = requireJsonBody(req);

                    auto id = catalogRepository->createBuildingForOrganization(
                        body["organizationId"].asInt(), body["name"].asString(),
                        body["address"].asString(), body["description"].asString());

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

        drogon::app().registerHandler(
            "/api/admin/rooms",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);

                    const auto body = requireJsonBody(req);

                    auto id = catalogRepository->createRoomForBuilding(
                        body["buildingId"].asInt(), body["roomName"].asString(),
                        body["roomType"].asString(), body["floorNumber"].asInt(),
                        doubleOrDefault(body, "minNormalPowerKw", 0.0),
                        doubleOrDefault(body, "maxNormalPowerKw", 2.8),
                        boolOrDefault(body, "allowUnattendedPower", false));

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

        drogon::app().registerHandler(
            "/api/admin/devices",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);

                    const auto body = requireJsonBody(req);

                    catalogRepository->createDeviceForRoom(
                        body["deviceId"].asString(), body["deviceName"].asString(),
                        body["deviceModel"].asString(), body["firmwareVersion"].asString(),
                        intOrDefault(body, "roomId", 0));

                    json["success"] = true;
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
            "/api/admin/buildings/update",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);
                    const auto body = requireJsonBody(req);

                    json["success"] = catalogRepository->updateBuilding(
                        body["id"].asInt(),
                        body["organizationId"].asInt(),
                        body["name"].asString(),
                        body["address"].asString(),
                        body["description"].asString());
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
            "/api/admin/buildings/delete",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);
                    const auto body = requireJsonBody(req);

                    json["success"] = catalogRepository->deleteBuilding(body["id"].asInt());
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
            "/api/admin/rooms/update",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);
                    const auto body = requireJsonBody(req);

                    json["success"] = catalogRepository->updateRoom(
                        body["id"].asInt(),
                        body["buildingId"].asInt(),
                        body["roomName"].asString(),
                        body["roomType"].asString(),
                        body["floorNumber"].asInt(),
                        doubleOrDefault(body, "minNormalPowerKw", 0.0),
                        doubleOrDefault(body, "maxNormalPowerKw", 2.8),
                        boolOrDefault(body, "allowUnattendedPower", false));
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
            "/api/admin/rooms/delete",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);
                    const auto body = requireJsonBody(req);

                    json["success"] = catalogRepository->deleteRoom(body["id"].asInt());
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
            "/api/admin/devices/update",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);
                    const auto body = requireJsonBody(req);

                    json["success"] = catalogRepository->updateDevice(
                        body["deviceId"].asString(),
                        body["deviceName"].asString(),
                        body["deviceModel"].asString(),
                        body["firmwareVersion"].asString(),
                        intOrDefault(body, "roomId", 0));
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
            "/api/admin/devices/delete",
            [catalogRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auth.requireAdmin(req);
                    const auto body = requireJsonBody(req);

                    json["success"] = catalogRepository->deleteDevice(body["deviceId"].asString());
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

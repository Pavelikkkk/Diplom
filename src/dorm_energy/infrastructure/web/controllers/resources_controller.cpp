#include "dorm_energy/infrastructure/web/controllers/resources_controller.hpp"

#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

namespace dorm_energy::web
{
    void registerResourceRoutes(const WebContext &context)
    {
        auto catalogRepository = context.catalogRepository;
        auto anomalyRepository = context.anomalyRepository;
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
    }
} // namespace dorm_energy::web

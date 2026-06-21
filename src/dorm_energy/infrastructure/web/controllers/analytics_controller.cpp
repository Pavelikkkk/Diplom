#include "dorm_energy/infrastructure/web/controllers/analytics_controller.hpp"

#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

namespace dorm_energy::web
{
    void registerAnalyticsRoutes(const WebContext &context)
    {
        auto repository = context.dashboardRepository;
        auto auth = AuthMiddleware(context.userRepository, context.authService);

        drogon::app().registerHandler(
            "/api/analytics/top-consumers",
            [repository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto consumers = repository->getTopConsumers(10, user.organizationId);

                    callback(makeJsonResponse(toJsonArray(consumers)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/analytics/anomalies-by-type",
            [repository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto stats = repository->getAnomalyStatistics(user.organizationId);

                    callback(makeJsonResponse(toJsonArray(stats)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/analytics/energy-by-room",
            [repository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto rooms = repository->getEnergyByRoom(user.organizationId);

                    callback(makeJsonResponse(toJsonArray(rooms)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/analytics/severity-distribution",
            [repository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto stats = repository->getSeverityDistribution(user.organizationId);

                    callback(makeJsonResponse(toJsonArray(stats)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/power/history",
            [repository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto points = repository->getPowerHistory(24, user.organizationId);

                    callback(makeJsonResponse(toJsonArray(points)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });
    }
} // namespace dorm_energy::web

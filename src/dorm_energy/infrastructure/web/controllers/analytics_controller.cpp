#include "dorm_energy/infrastructure/web/controllers/analytics_controller.hpp"

#include "dorm_energy/domain/storage/repositories/isubscription_repository.hpp"
#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

#include <algorithm>
#include <stdexcept>

namespace dorm_energy::web
{
    namespace
    {
        int intQueryParam(
            const drogon::HttpRequestPtr &req,
            const std::string &name,
            int fallback = 0)
        {
            const auto value = req->getParameter(name);

            if (value.empty())
            {
                return fallback;
            }

            return std::stoi(value);
        }

        UserDto requireBusinessUser(
            const AuthMiddleware &auth,
            const std::shared_ptr<storage::ISubscriptionRepository> &subscriptionRepository,
            const drogon::HttpRequestPtr &req)
        {
            auto user = auth.requireUser(req);
            const auto subscription = subscriptionRepository->getUserSubscription(user.id);
            const auto hasAnalyticsPlan =
                subscription.plan == "PRO" ||
                subscription.plan == "BUSINESS" ||
                subscription.plan == "ENTERPRISE";

            if (user.accountType != "BUSINESS" && user.role != "ADMIN" && !hasAnalyticsPlan)
            {
                throw std::runtime_error("Business subscription is required");
            }

            return user;
        }
    }

    void registerAnalyticsRoutes(const WebContext &context)
    {
        auto repository = context.dashboardRepository;
        auto subscriptionRepository = context.subscriptionRepository;
        auto auth = AuthMiddleware(context.userRepository, context.authService);

        drogon::app().registerHandler(
            "/api/analytics/top-consumers",
            [repository, subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = requireBusinessUser(auth, subscriptionRepository, req);
                    const auto buildingId = intQueryParam(req, "buildingId");
                    auto consumers = repository->getTopConsumers(10, user.organizationId, buildingId);

                    callback(makeJsonResponse(toJsonArray(consumers)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/analytics/anomalies-by-type",
            [repository, subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = requireBusinessUser(auth, subscriptionRepository, req);
                    const auto buildingId = intQueryParam(req, "buildingId");
                    auto stats = repository->getAnomalyStatistics(user.organizationId, buildingId);

                    callback(makeJsonResponse(toJsonArray(stats)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/analytics/energy-by-room",
            [repository, subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = requireBusinessUser(auth, subscriptionRepository, req);
                    const auto buildingId = intQueryParam(req, "buildingId");
                    auto rooms = repository->getEnergyByRoom(user.organizationId, buildingId);

                    callback(makeJsonResponse(toJsonArray(rooms)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/analytics/severity-distribution",
            [repository, subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = requireBusinessUser(auth, subscriptionRepository, req);
                    const auto buildingId = intQueryParam(req, "buildingId");
                    auto stats = repository->getSeverityDistribution(user.organizationId, buildingId);

                    callback(makeJsonResponse(toJsonArray(stats)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });

        drogon::app().registerHandler(
            "/api/power/history",
            [repository, subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = requireBusinessUser(auth, subscriptionRepository, req);
                    int hours = 24;

                    if (const auto value = req->getParameter("hours"); !value.empty())
                    {
                        hours = std::clamp(std::stoi(value), 1, 24 * 31);
                    }

                    const auto buildingId = intQueryParam(req, "buildingId");
                    auto points = repository->getPowerHistory(hours, user.organizationId, buildingId);

                    callback(makeJsonResponse(toJsonArray(points)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });
    }
} // namespace dorm_energy::web

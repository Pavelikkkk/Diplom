#include "dorm_energy/infrastructure/web/controllers/account_controller.hpp"

#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

namespace dorm_energy::web
{
    void registerAccountRoutes(const WebContext &context)
    {
        auto userRepository = context.userRepository;
        auto subscriptionRepository = context.subscriptionRepository;
        auto auth = AuthMiddleware(userRepository, context.authService);

        drogon::app().registerHandler(
            "/api/account",
            [subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                                           std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto user = auth.requireAuthenticatedUser(req);
                    auto subscription = subscriptionRepository->getUserSubscription(user.id);

                    json = toJson(user);
                    json["subscription"] = toJson(subscription);
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
            "/api/account/telegram-chat-id",
            [userRepository, auth](const drogon::HttpRequestPtr &req,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto claims = auth.requireClaims(req);
                    auto body = req->getJsonObject();

                    if (!body)
                    {
                        throw std::runtime_error("Invalid JSON body");
                    }

                    auto updated = userRepository->updateUserTelegramChatId(
                        claims.userId, (*body)["telegramChatId"].asString());

                    if (!updated)
                    {
                        throw std::runtime_error("User not found");
                    }

                    json["success"] = true;
                }
                catch (const std::exception &ex)
                {
                    json = makeErrorJson(ex);
                    status = statusForError(ex.what());
                }

                callback(makeJsonResponse(json, status));
            },
            {drogon::Put});

        drogon::app().registerHandler(
            "/api/subscription",
            [subscriptionRepository, auth](const drogon::HttpRequestPtr &req,
                                           std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto claims = auth.requireClaims(req);

                    json = toJson(subscriptionRepository->getUserSubscription(claims.userId));
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
    }
} // namespace dorm_energy::web

#include "dorm_energy/infrastructure/web/controllers/auth_controller.hpp"

#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

namespace dorm_energy::web
{
    void registerAuthRoutes(const WebContext &context)
    {
        auto authService = context.authService;
        auto auth = AuthMiddleware(context.userRepository, authService);

        drogon::app().registerHandler(
            "/api/auth/me",
            [auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto user = auth.requireAuthenticatedUser(req);

                    json = toJson(user);
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
            "/api/auth/register",
            [authService](const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto body = req->getJsonObject();

                    if (!body)
                    {
                        throw std::runtime_error("Invalid JSON body");
                    }

                    auto accountType = (*body)["accountType"].asString();

                    if (accountType.empty())
                    {
                        accountType = "PERSONAL";
                    }

                    const dorm_energy::auth::RegisterRequest registerRequest{
                        .username = (*body)["username"].asString(),
                        .email = (*body)["email"].asString(),
                        .password = (*body)["password"].asString(),
                        .accountType = accountType,
                    };

                    auto userId = authService->registerUser(registerRequest);

                    json["success"] = true;
                    json["userId"] = userId;
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
            "/api/auth/login",
            [authService](const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                Json::Value json;
                auto status = drogon::k200OK;

                try
                {
                    auto body = req->getJsonObject();

                    if (!body)
                    {
                        throw std::runtime_error("Invalid JSON body");
                    }

                    const dorm_energy::auth::LoginRequest loginRequest{
                        .email = (*body)["email"].asString(),
                        .password = (*body)["password"].asString(),
                    };

                    auto authResponse = authService->loginUser(loginRequest);

                    json["token"] = authResponse.token;
                    json["success"] = true;
                }
                catch (const std::exception &ex)
                {
                    json = makeErrorJson(ex);
                    status = drogon::k401Unauthorized;
                }

                callback(makeJsonResponse(json, status));
            },
            {drogon::Post});

    }
} // namespace dorm_energy::web

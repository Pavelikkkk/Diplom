#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

namespace dorm_energy::web
{
    void addCorsHeaders(const drogon::HttpResponsePtr &response)
    {
        response->addHeader("Access-Control-Allow-Origin", "*");
    }

    drogon::HttpStatusCode statusForError(const std::string &error)
    {
        if (error == "Missing token" || error == "User not found" ||
            error.find("token") != std::string::npos ||
            error.find("Invalid input") != std::string::npos ||
            error.find("signature") != std::string::npos ||
            error.find("issuer") != std::string::npos)
        {
            return drogon::k401Unauthorized;
        }

        if (error == "Admin role is required" ||
            error == "Business subscription is required")
        {
            return drogon::k403Forbidden;
        }

        return drogon::k400BadRequest;
    }

    drogon::HttpResponsePtr makeJsonResponse(const Json::Value &json, drogon::HttpStatusCode status)
    {
        auto response = drogon::HttpResponse::newHttpJsonResponse(json);
        response->setStatusCode(status);
        addCorsHeaders(response);

        return response;
    }

    Json::Value makeErrorJson(const std::exception &ex)
    {
        Json::Value json;

        json["success"] = false;
        json["error"] = ex.what();

        return json;
    }
} // namespace dorm_energy::web

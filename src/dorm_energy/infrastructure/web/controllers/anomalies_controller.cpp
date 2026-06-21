#include "dorm_energy/infrastructure/web/controllers/anomalies_controller.hpp"

#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"
#include "dorm_energy/infrastructure/web/utils/dto_json_mapper.hpp"
#include "dorm_energy/infrastructure/web/utils/json_response.hpp"

#include <drogon/drogon.h>

namespace dorm_energy::web
{
    void registerAnomalyRoutes(const WebContext &context)
    {
        auto repository = context.anomalyRepository;
        auto auth = AuthMiddleware(context.userRepository, context.authService);

        drogon::app().registerHandler(
            "/api/anomalies/latest",
            [repository, auth](const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback)
            {
                try
                {
                    auto user = auth.requireUser(req);
                    auto latest = repository->getLatestAnomalies(20, user.organizationId);

                    callback(makeJsonResponse(toJsonArray(latest)));
                }
                catch (const std::exception &ex)
                {
                    callback(makeJsonResponse(makeErrorJson(ex), statusForError(ex.what())));
                }
            });
    }
} // namespace dorm_energy::web

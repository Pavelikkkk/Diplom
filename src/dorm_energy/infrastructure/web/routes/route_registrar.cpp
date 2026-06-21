#include "dorm_energy/infrastructure/web/routes/route_registrar.hpp"

#include "dorm_energy/infrastructure/web/controllers/admin_controller.hpp"
#include "dorm_energy/infrastructure/web/controllers/account_controller.hpp"
#include "dorm_energy/infrastructure/web/controllers/analytics_controller.hpp"
#include "dorm_energy/infrastructure/web/controllers/anomalies_controller.hpp"
#include "dorm_energy/infrastructure/web/controllers/auth_controller.hpp"
#include "dorm_energy/infrastructure/web/controllers/health_controller.hpp"
#include "dorm_energy/infrastructure/web/controllers/resources_controller.hpp"

namespace dorm_energy::web
{
    void registerWebRoutes(const WebContext &context)
    {
        registerHealthRoutes();
        registerAuthRoutes(context);
        registerAccountRoutes(context);
        registerAdminRoutes(context);
        registerResourceRoutes(context);
        registerAnomalyRoutes(context);
        registerAnalyticsRoutes(context);
    }
} // namespace dorm_energy::web

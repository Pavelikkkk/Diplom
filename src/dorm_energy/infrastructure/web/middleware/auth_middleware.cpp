#include "dorm_energy/infrastructure/web/middleware/auth_middleware.hpp"

#include <stdexcept>

namespace dorm_energy::web
{
    AuthMiddleware::AuthMiddleware(
        std::shared_ptr<dorm_energy::storage::IUserRepository> repository,
        std::shared_ptr<dorm_energy::auth::AuthService> authService)
        : repository_(std::move(repository)), authService_(std::move(authService))
    {
    }

    dorm_energy::auth::UserClaims AuthMiddleware::requireClaims(
        const drogon::HttpRequestPtr &req) const
    {
        auto auth = req->getHeader("Authorization");
        const std::string prefix = "Bearer ";

        if (auth.rfind(prefix, 0) != 0)
        {
            throw std::runtime_error("Missing token");
        }

        return authService_->validateToken(auth.substr(prefix.size()));
    }

    UserDto AuthMiddleware::requireAuthenticatedUser(const drogon::HttpRequestPtr &req) const
    {
        auto claims = requireClaims(req);
        auto user = repository_->findUserById(claims.userId);

        if (!user)
        {
            throw std::runtime_error("User not found");
        }

        return *user;
    }

    UserDto AuthMiddleware::requireUser(const drogon::HttpRequestPtr &req) const
    {
        auto user = requireAuthenticatedUser(req);

        if (user.organizationId <= 0)
        {
            throw std::runtime_error("User organization is not configured");
        }

        return user;
    }

    UserDto AuthMiddleware::requireAdmin(const drogon::HttpRequestPtr &req) const
    {
        auto user = requireUser(req);

        if (user.role != "ADMIN")
        {
            throw std::runtime_error("Admin role is required");
        }

        return user;
    }
} // namespace dorm_energy::web

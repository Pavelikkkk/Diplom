#pragma once

#include "dorm_energy/application/auth/auth_service.hpp"
#include "dorm_energy/domain/auth/user_claims.hpp"
#include "dorm_energy/domain/storage/dto/user_dto.hpp"
#include "dorm_energy/domain/storage/repositories/iuser_repository.hpp"

#include <drogon/drogon.h>

#include <memory>

namespace dorm_energy::web
{
    class AuthMiddleware
    {
    public:
        AuthMiddleware(std::shared_ptr<dorm_energy::storage::IUserRepository> repository,
                       std::shared_ptr<dorm_energy::auth::AuthService> authService);

        dorm_energy::auth::UserClaims requireClaims(const drogon::HttpRequestPtr &req) const;
        UserDto requireAuthenticatedUser(const drogon::HttpRequestPtr &req) const;
        UserDto requireUser(const drogon::HttpRequestPtr &req) const;
        UserDto requireAdmin(const drogon::HttpRequestPtr &req) const;

    private:
        std::shared_ptr<dorm_energy::storage::IUserRepository> repository_;
        std::shared_ptr<dorm_energy::auth::AuthService> authService_;
    };
} // namespace dorm_energy::web

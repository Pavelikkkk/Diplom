#pragma once

#include "dorm_energy/application/auth/auth_dto.hpp"
#include "dorm_energy/domain/auth/ijwt_service.hpp"
#include "dorm_energy/domain/auth/ipassword_hasher.hpp"
#include "dorm_energy/domain/auth/user_claims.hpp"
#include "dorm_energy/domain/storage/repositories/iuser_repository.hpp"

#include <memory>
#include <string>

namespace dorm_energy::auth
{
    class AuthService
    {
    public:
        AuthService(
            std::shared_ptr<storage::IUserRepository> repository,
            std::shared_ptr<IPasswordHasher> passwordHasher,
            std::shared_ptr<IJwtService> jwtService);

        int registerUser(
            const RegisterRequest &request);

        AuthResponse loginUser(
            const LoginRequest &request);

        UserClaims validateToken(
            const std::string &token);

    private:
        std::shared_ptr<storage::IUserRepository> repository_;

        std::shared_ptr<IPasswordHasher> passwordHasher_;

        std::shared_ptr<IJwtService> jwtService_;
    };
}
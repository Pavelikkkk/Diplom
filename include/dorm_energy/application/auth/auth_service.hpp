#pragma once

#include "dorm_energy/domain/auth/ijwt_service.hpp"
#include "dorm_energy/domain/auth/ipassword_hasher.hpp"
#include "dorm_energy/domain/storage/imeasurement_repository.hpp"

#include <memory>
#include <string>

class AuthService
{
public:
    AuthService(
        std::shared_ptr<dorm_energy::storage::IMeasurementRepository> repository,
        std::shared_ptr<IPasswordHasher> passwordHasher,
        std::shared_ptr<IJwtService> jwtService);

    int registerUser( // почему не дто
        const std::string &username,
        const std::string &email,
        const std::string &password,
        const std::string &accountType);

    std::string loginUser(
        const std::string &email,
        const std::string &password);

    UserClaims validateToken(
        const std::string &token);

private:
    std::shared_ptr<dorm_energy::storage::IMeasurementRepository> repository_;

    std::shared_ptr<IPasswordHasher> passwordHasher_;

    std::shared_ptr<IJwtService> jwtService_;
};

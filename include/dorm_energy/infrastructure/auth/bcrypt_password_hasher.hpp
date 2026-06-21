#pragma once

#include "dorm_energy/domain/auth/ipassword_hasher.hpp"

namespace dorm_energy::auth
{
    class BcryptPasswordHasher final : public IPasswordHasher
    {
    public:
        std::string hash(
            const std::string &password) override;

        bool verify(
            const std::string &password,
            const std::string &passwordHash) override;
    };
}
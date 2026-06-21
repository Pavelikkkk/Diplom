#pragma once

#include <string>

namespace dorm_energy::auth
{
    class IPasswordHasher
    {
    public:
        virtual ~IPasswordHasher() = default;

        virtual std::string hash(
            const std::string &password) = 0;

        virtual bool verify(
            const std::string &password,
            const std::string &hash) = 0;
    };
}
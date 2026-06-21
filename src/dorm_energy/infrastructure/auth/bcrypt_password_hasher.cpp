#include "dorm_energy/infrastructure/auth/bcrypt_password_hasher.hpp"

#include <bcrypt/bcrypt.h>

namespace dorm_energy::auth
{
    std::string BcryptPasswordHasher::hash(
        const std::string &password)
    {
        return bcrypt::generateHash(password);
    }

    bool BcryptPasswordHasher::verify(
        const std::string &password,
        const std::string &passwordHash)
    {
        return bcrypt::validatePassword(
            password,
            passwordHash);
    }
} // namespace dorm_energy::auth

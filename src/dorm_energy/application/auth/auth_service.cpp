#include "dorm_energy/application/auth/auth_service.hpp"

#include <stdexcept>

AuthService::AuthService(std::shared_ptr<dorm_energy::storage::IMeasurementRepository> repository,
                         std::shared_ptr<IPasswordHasher> passwordHasher,
                         std::shared_ptr<IJwtService> jwtService)
    : repository_(std::move(repository)), passwordHasher_(std::move(passwordHasher)),
      jwtService_(std::move(jwtService))
{
}

int AuthService::registerUser(const std::string &username, const std::string &email,
                              const std::string &password, const std::string &accountType)
{
    if (username.size() < 3)
    {
        throw std::runtime_error("Username must be at least 3 characters");
    }

    if (email.empty() || email.find('@') == std::string::npos)
    {
        throw std::runtime_error("Invalid email");
    }

    if (password.size() < 8)
    {
        throw std::runtime_error("Password must be at least 8 characters");
    }

    auto existing = repository_->findUserByEmail(email);

    if (existing.has_value())
    {
        throw std::runtime_error("User already exists");
    }

    UserDto user;

    user.username = username;
    user.email = email;
    user.passwordHash = passwordHasher_->hash(password);
    user.role = "USER";
    user.organizationId = 0;
    user.accountType = accountType == "BUSINESS" ? "BUSINESS" : "PERSONAL";

    return repository_->createUser(user);
}

std::string AuthService::loginUser(const std::string &email, const std::string &password)
{
    auto user = repository_->findUserByEmail(email);

    if (!user.has_value())
    {
        throw std::runtime_error("Invalid credentials");
    }

    const bool valid = passwordHasher_->verify(password, user->passwordHash);

    if (!valid)
    {
        throw std::runtime_error("Invalid credentials");
    }

    return jwtService_->generateToken(user->id, user->email, user->role);
}

UserClaims AuthService::validateToken(const std::string &token)
{
    return jwtService_->validateToken(token);
}

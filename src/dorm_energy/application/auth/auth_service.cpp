#include "dorm_energy/application/auth/auth_service.hpp"

#include "dorm_energy/domain/storage/dto/user_dto.hpp"

#include <stdexcept>
#include <utility>

namespace dorm_energy::auth
{
    namespace
    {
        void validateRegisterRequest(
            const RegisterRequest &request)
        {
            if (request.username.size() < 3)
            {
                throw std::runtime_error("Username must be at least 3 characters");
            }

            if (request.email.empty() || request.email.find('@') == std::string::npos)
            {
                throw std::runtime_error("Invalid email");
            }

            if (request.password.size() < 8)
            {
                throw std::runtime_error("Password must be at least 8 characters");
            }
        }

        std::string normalizeAccountType(
            const std::string &accountType)
        {
            return accountType == "BUSINESS" ? "BUSINESS" : "PERSONAL";
        }
    }

    AuthService::AuthService(
        std::shared_ptr<storage::IUserRepository> repository,
        std::shared_ptr<IPasswordHasher> passwordHasher,
        std::shared_ptr<IJwtService> jwtService)
        : repository_(std::move(repository)),
          passwordHasher_(std::move(passwordHasher)),
          jwtService_(std::move(jwtService))
    {
        if (!repository_)
        {
            throw std::invalid_argument("repository must not be null");
        }

        if (!passwordHasher_)
        {
            throw std::invalid_argument("passwordHasher must not be null");
        }

        if (!jwtService_)
        {
            throw std::invalid_argument("jwtService must not be null");
        }
    }

    int AuthService::registerUser(
        const RegisterRequest &request)
    {
        validateRegisterRequest(request);

        const auto existing = repository_->findUserByEmail(request.email);

        if (existing.has_value())
        {
            throw std::runtime_error("User already exists");
        }

        UserDto user;
        user.username = request.username;
        user.email = request.email;
        user.passwordHash = passwordHasher_->hash(request.password);
        user.role = "USER";
        user.organizationId = 0;
        user.accountType = normalizeAccountType(request.accountType);

        return repository_->createUser(user);
    }

    AuthResponse AuthService::loginUser(
        const LoginRequest &request)
    {
        const auto user = repository_->findUserByEmail(request.email);

        if (!user.has_value())
        {
            throw std::runtime_error("Invalid credentials");
        }

        const bool valid = passwordHasher_->verify(request.password, user->passwordHash);

        if (!valid)
        {
            throw std::runtime_error("Invalid credentials");
        }

        AuthResponse response;
        response.token = jwtService_->generateToken(
            user->id,
            user->email,
            user->role);

        return response;
    }

    UserClaims AuthService::validateToken(
        const std::string &token)
    {
        return jwtService_->validateToken(token);
    }
} // namespace dorm_energy::auth

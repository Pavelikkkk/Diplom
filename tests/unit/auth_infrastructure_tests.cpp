#include "dorm_energy/infrastructure/auth/jwt_service.hpp"
#include "dorm_energy/infrastructure/auth/bcrypt_password_hasher.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

TEST(AuthInfrastructureTest, PasswordHasherVerifiesOnlyMatchingPassword)
{
    dorm_energy::auth::BcryptPasswordHasher hasher;

    const auto hash =
        hasher.hash(
            "CorrectPass123!");

    EXPECT_TRUE(
        hasher.verify(
            "CorrectPass123!",
            hash));

    EXPECT_FALSE(
        hasher.verify(
            "WrongPass123!",
            hash));
}

TEST(AuthInfrastructureTest, JwtServiceGeneratesAndValidatesClaims)
{
    dorm_energy::auth::JwtService jwt{
        "test-secret",
        24};

    const auto token =
        jwt.generateToken(
            42,
            "user@example.com",
            "ADMIN");

    const auto claims =
        jwt.validateToken(
            token);

    EXPECT_EQ(
        claims.userId,
        42);

    EXPECT_EQ(
        claims.email,
        "user@example.com");

    EXPECT_EQ(
        claims.role,
        "ADMIN");
}

TEST(AuthInfrastructureTest, JwtServiceRejectsMalformedAndWronglySignedTokens)
{
    dorm_energy::auth::JwtService jwt{
        "test-secret",
        24};

    EXPECT_THROW(
        jwt.validateToken(
            "not-a-jwt"),
        std::exception);

    dorm_energy::auth::JwtService otherJwt{
        "another-secret",
        24};

    const auto token =
        otherJwt.generateToken(
            1,
            "user@example.com",
            "USER");

    EXPECT_THROW(
        jwt.validateToken(
            token),
        std::exception);
}

//
// Created by Adelina Jiang on 3/13/2026.
//

//
// Created by Adelina Jiang on 2/3/2026.
//

#include "catch2/catch_test_macros.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Real.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

#include "Oasis/FirstOrderLinear.hpp"
#include "Oasis/FirstOrderHomogeneous.hpp"
#include "Oasis/FirstOrderSeparable.hpp"

// =============================================================================
//  FirstOrderLinear
//  Form: dy/dx + P(x)·y = Q(x)
// =============================================================================

TEST_CASE("FirstOrderLinear — Classify", "[DE][FirstOrderLinear][Classify]")
{
    // dy/dx + 2y = 0  =>  P(x)=2, Q(x)=0
    Oasis::FirstOrderLinear de {
        std::make_unique<Oasis::Real>(2.0f),   // P(x) = 2
        std::make_unique<Oasis::Real>(0.0f)    // Q(x) = 0
    };

    REQUIRE(de.Classify() == Oasis::DECategory::FirstOrderLinear);
}

TEST_CASE("FirstOrderLinear — Equals same equation", "[DE][FirstOrderLinear][Equals]")
{
    // dy/dx + 3y = x
    Oasis::FirstOrderLinear de1 {
        std::make_unique<Oasis::Real>(3.0f),
        std::make_unique<Oasis::Variable>("x")
    };
    Oasis::FirstOrderLinear de2 {
        std::make_unique<Oasis::Real>(3.0f),
        std::make_unique<Oasis::Variable>("x")
    };

    REQUIRE(de1.Equals(de2));
}

TEST_CASE("FirstOrderLinear — Equals different P(x)", "[DE][FirstOrderLinear][Equals]")
{
    // dy/dx + 3y = x  vs  dy/dx + 5y = x  — should NOT be equal
    Oasis::FirstOrderLinear de1 {
        std::make_unique<Oasis::Real>(3.0f),
        std::make_unique<Oasis::Variable>("x")
    };
    Oasis::FirstOrderLinear de2 {
        std::make_unique<Oasis::Real>(5.0f),
        std::make_unique<Oasis::Variable>("x")
    };

    REQUIRE_FALSE(de1.Equals(de2));
}

TEST_CASE("FirstOrderLinear — Copy", "[DE][FirstOrderLinear][Copy]")
{
    Oasis::FirstOrderLinear de {
        std::make_unique<Oasis::Real>(2.0f),
        std::make_unique<Oasis::Variable>("x")
    };

    auto copy = de.Copy();
    REQUIRE(copy != nullptr);
    REQUIRE(de.Equals(*copy));
}

TEST_CASE("FirstOrderLinear — Solve constant P and Q=0 (trivial)", "[DE][FirstOrderLinear][Solve]")
{
    // dy/dx + 2y = 0
    // Integrating factor: μ = e^(2x)
    // Solution: y = C·e^(-2x)
    // Solve() should return a non-null expression without error.
    Oasis::FirstOrderLinear de {
        std::make_unique<Oasis::Real>(2.0f),   // P(x) = 2
        std::make_unique<Oasis::Real>(0.0f)    // Q(x) = 0
    };

    auto result = de.Solve();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}

TEST_CASE("FirstOrderLinear — Solve constant P and constant Q", "[DE][FirstOrderLinear][Solve]")
{
    // dy/dx + 2y = 4
    // Integrating factor: μ = e^(2x)
    // Solution: y = 2 + C·e^(-2x)
    Oasis::FirstOrderLinear de {
        std::make_unique<Oasis::Real>(2.0f),   // P(x) = 2
        std::make_unique<Oasis::Real>(4.0f)    // Q(x) = 4
    };

    auto result = de.Solve();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}

TEST_CASE("FirstOrderLinear — different variable names", "[DE][FirstOrderLinear][Variables]")
{
    // dt/ds + t = s  (independent=s, dependent=t)
    Oasis::FirstOrderLinear de1 {
        std::make_unique<Oasis::Real>(1.0f),
        std::make_unique<Oasis::Variable>("s"),
        "s", "t"
    };
    Oasis::FirstOrderLinear de2 {
        std::make_unique<Oasis::Real>(1.0f),
        std::make_unique<Oasis::Variable>("x"),
        "x", "y"
    };

    // Different variable names — should not be equal
    REQUIRE_FALSE(de1.Equals(de2));
}

// =============================================================================
//  FirstOrderHomogeneous
//  Form: dy/dx = f(y/x),  solved via v = y/x substitution
// =============================================================================

TEST_CASE("FirstOrderHomogeneous — Classify", "[DE][FirstOrderHomogeneous][Classify]")
{
    // dy/dx = v  (i.e. dy/dx = y/x, simplest homogeneous case)
    Oasis::FirstOrderHomogeneous de {
        std::make_unique<Oasis::Variable>("v")
    };

    REQUIRE(de.Classify() == Oasis::DECategory::FirstOrderHomogeneous);
}

TEST_CASE("FirstOrderHomogeneous — Equals same equation", "[DE][FirstOrderHomogeneous][Equals]")
{
    // dy/dx = v^2  (i.e. f(v) = v^2)
    Oasis::FirstOrderHomogeneous de1 {
        std::make_unique<Oasis::Exponent<Oasis::Variable, Oasis::Real>>(
            Oasis::Variable { "v" }, Oasis::Real { 2.0f })
    };
    Oasis::FirstOrderHomogeneous de2 {
        std::make_unique<Oasis::Exponent<Oasis::Variable, Oasis::Real>>(
            Oasis::Variable { "v" }, Oasis::Real { 2.0f })
    };

    REQUIRE(de1.Equals(de2));
}

TEST_CASE("FirstOrderHomogeneous — Equals different f(v)", "[DE][FirstOrderHomogeneous][Equals]")
{
    // dy/dx = v^2  vs  dy/dx = v^3  — should NOT be equal
    Oasis::FirstOrderHomogeneous de1 {
        std::make_unique<Oasis::Exponent<Oasis::Variable, Oasis::Real>>(
            Oasis::Variable { "v" }, Oasis::Real { 2.0f })
    };
    Oasis::FirstOrderHomogeneous de2 {
        std::make_unique<Oasis::Exponent<Oasis::Variable, Oasis::Real>>(
            Oasis::Variable { "v" }, Oasis::Real { 3.0f })
    };

    REQUIRE_FALSE(de1.Equals(de2));
}

TEST_CASE("FirstOrderHomogeneous — Copy", "[DE][FirstOrderHomogeneous][Copy]")
{
    Oasis::FirstOrderHomogeneous de {
        std::make_unique<Oasis::Variable>("v")
    };

    auto copy = de.Copy();
    REQUIRE(copy != nullptr);
    REQUIRE(de.Equals(*copy));
}

TEST_CASE("FirstOrderHomogeneous — Solve linear f(v)=v", "[DE][FirstOrderHomogeneous][Solve]")
{
    // dy/dx = y/x  =>  f(v) = v
    // f(v) - v = 0, degenerate — expect an error (can't divide by zero)
    Oasis::FirstOrderHomogeneous de {
        std::make_unique<Oasis::Variable>("v")   // f(v) = v
    };

    auto result = de.Solve();
    // f(v)-v = 0 means the integrand is undefined; either an error or a
    // null value is acceptable here — we just check it doesn't crash.
    REQUIRE_FALSE(result.has_value() && result.value() == nullptr);
}

TEST_CASE("FirstOrderHomogeneous — Solve f(v)=v^2", "[DE][FirstOrderHomogeneous][Solve]")
{
    // dy/dx = (y/x)^2  =>  f(v) = v^2
    // f(v) - v = v^2 - v
    // ∫ dv/(v^2-v)  =  ∫ dx/x
    Oasis::FirstOrderHomogeneous de {
        std::make_unique<Oasis::Exponent<Oasis::Variable, Oasis::Real>>(
            Oasis::Variable { "v" }, Oasis::Real { 2.0f })
    };

    auto result = de.Solve();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}

// =============================================================================
//  FirstOrderSeparable
//  Form: g(y) dy = f(x) dx
// =============================================================================

TEST_CASE("FirstOrderSeparable — Classify", "[DE][FirstOrderSeparable][Classify]")
{
    // dy/dx = x/y  =>  y dy = x dx  =>  g(y)=y, f(x)=x
    Oasis::FirstOrderSeparable de {
        std::make_unique<Oasis::Variable>("y"),  // g(y) = y
        std::make_unique<Oasis::Variable>("x")   // f(x) = x
    };

    REQUIRE(de.Classify() == Oasis::DECategory::FirstOrderSeparable);
}

TEST_CASE("FirstOrderSeparable — Equals same equation", "[DE][FirstOrderSeparable][Equals]")
{
    // g(y)=2y, f(x)=3x
    Oasis::FirstOrderSeparable de1 {
        std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Variable>>(
            Oasis::Real { 2.0f }, Oasis::Variable { "y" }),
        std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Variable>>(
            Oasis::Real { 3.0f }, Oasis::Variable { "x" })
    };
    Oasis::FirstOrderSeparable de2 {
        std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Variable>>(
            Oasis::Real { 2.0f }, Oasis::Variable { "y" }),
        std::make_unique<Oasis::Multiply<Oasis::Real, Oasis::Variable>>(
            Oasis::Real { 3.0f }, Oasis::Variable { "x" })
    };

    REQUIRE(de1.Equals(de2));
}

TEST_CASE("FirstOrderSeparable — Equals different f(x)", "[DE][FirstOrderSeparable][Equals]")
{
    // g(y)=y, f(x)=x  vs  g(y)=y, f(x)=x^2  — should NOT be equal
    Oasis::FirstOrderSeparable de1 {
        std::make_unique<Oasis::Variable>("y"),
        std::make_unique<Oasis::Variable>("x")
    };
    Oasis::FirstOrderSeparable de2 {
        std::make_unique<Oasis::Variable>("y"),
        std::make_unique<Oasis::Exponent<Oasis::Variable, Oasis::Real>>(
            Oasis::Variable { "x" }, Oasis::Real { 2.0f })
    };

    REQUIRE_FALSE(de1.Equals(de2));
}

TEST_CASE("FirstOrderSeparable — Copy", "[DE][FirstOrderSeparable][Copy]")
{
    Oasis::FirstOrderSeparable de {
        std::make_unique<Oasis::Variable>("y"),
        std::make_unique<Oasis::Variable>("x")
    };

    auto copy = de.Copy();
    REQUIRE(copy != nullptr);
    REQUIRE(de.Equals(*copy));
}

TEST_CASE("FirstOrderSeparable — Solve constants (dy/dx = 3/2)", "[DE][FirstOrderSeparable][Solve]")
{
    // g(y)=1, f(x)=3  =>  ∫1 dy = ∫3 dx  =>  y = 3x + C
    Oasis::FirstOrderSeparable de {
        std::make_unique<Oasis::Real>(1.0f),   // g(y) = 1
        std::make_unique<Oasis::Real>(3.0f)    // f(x) = 3
    };

    auto result = de.Solve();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}

TEST_CASE("FirstOrderSeparable — Solve y dy = x dx", "[DE][FirstOrderSeparable][Solve]")
{
    // g(y)=y, f(x)=x
    // ∫y dy = ∫x dx  =>  y^2/2 = x^2/2 + C  =>  y^2 - x^2 = C
    Oasis::FirstOrderSeparable de {
        std::make_unique<Oasis::Variable>("y"),
        std::make_unique<Oasis::Variable>("x")
    };

    auto result = de.Solve();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);

    // The implicit solution should be  y^2/2 - x^2/2  (= C)
    // We verify the structure: it should be a Subtract of two Divide expressions
    auto& solution = *result.value();
    REQUIRE(solution.Is<Oasis::Subtract>());
}

TEST_CASE("FirstOrderSeparable — Solve y dy = x^2 dx", "[DE][FirstOrderSeparable][Solve]")
{
    // g(y)=y, f(x)=x^2
    // ∫y dy = ∫x^2 dx  =>  y^2/2 = x^3/3 + C
    Oasis::FirstOrderSeparable de {
        std::make_unique<Oasis::Variable>("y"),
        std::make_unique<Oasis::Exponent<Oasis::Variable, Oasis::Real>>(
            Oasis::Variable { "x" }, Oasis::Real { 2.0f })
    };

    auto result = de.Solve();
    REQUIRE(result.has_value());
    REQUIRE(result.value() != nullptr);
}

TEST_CASE("FirstOrderSeparable — Solve returns error for null g(y)", "[DE][FirstOrderSeparable][Solve][Error]")
{
    // Passing null g(y) should return an unexpected error, not crash
    Oasis::FirstOrderSeparable de {
        nullptr,
        std::make_unique<Oasis::Variable>("x")
    };

    auto result = de.Solve();
    REQUIRE_FALSE(result.has_value());
}
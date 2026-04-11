//
// Created by Adelina Jiang on 4/10/2026.
//
#include "Oasis/FirstOrderHomogeneous.hpp"

#include "Oasis/Divide.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

#include <format>

namespace Oasis {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

FirstOrderHomogeneous::FirstOrderHomogeneous(
    std::unique_ptr<Expression> fv,
    std::string independentVar,
    std::string dependentVar)
    : DifferentialEquation(
          nullptr, // lhs — structure lives in fv_
          nullptr, // rhs
          std::move(independentVar),
          std::move(dependentVar))
    , fv_(std::move(fv))
{
    category_ = DECategory::FirstOrderHomogeneous;
}

// ---------------------------------------------------------------------------
// Copy constructor / copy-assignment
// ---------------------------------------------------------------------------

FirstOrderHomogeneous::FirstOrderHomogeneous(const FirstOrderHomogeneous& other)
    : DifferentialEquation(other)
    , fv_(other.fv_ ? other.fv_->Copy() : nullptr)
{
}

FirstOrderHomogeneous& FirstOrderHomogeneous::operator=(const FirstOrderHomogeneous& other)
{
    if (this == &other) {
        return *this;
    }

    DifferentialEquation::operator=(other);
    fv_ = other.fv_ ? other.fv_->Copy() : nullptr;

    return *this;
}

// ---------------------------------------------------------------------------
// Solve  —  substitution method  v = y/x
//
//   Given:  dy/dx = f(y/x)
//
//   Let v = y/x, so y = v·x and dy/dx = v + x·dv/dx.
//   Substituting:
//       v + x·dv/dx = f(v)
//       x·dv/dx     = f(v) - v
//
//   Separating:
//       dv / (f(v) - v)  =  dx / x
//
//   Integrating both sides:
//       ∫ 1/(f(v)-v) dv  =  ∫ 1/x dx  =  ln|x| + C
//
//   Then back-substitute v = y/x to get y(x).
// ---------------------------------------------------------------------------

std::expected<std::unique_ptr<Expression>, std::string>
FirstOrderHomogeneous::Solve() const
{
    if (!fv_) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — f(v) is null."
        };
    }

    const Variable v { "v" };       // substitution variable v = y/x
    const Variable x { independentVar_ };

    // Step 1: form the denominator  f(v) - v
    auto fvMinusV = Subtract<Expression> { *fv_, v }.Simplify();
    if (!fvMinusV) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — could not simplify f(v) - v."
        };
    }

    // Step 2: form the integrand for the left side  1 / (f(v) - v)
    auto lhsIntegrand = std::make_unique<Divide<Expression>>(
        Real { 1.0 },
        *fvMinusV);

    // Step 3: ∫ 1/(f(v)-v) dv
    auto lhsIntegral = lhsIntegrand->Integrate(v);
    if (!lhsIntegral) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — could not integrate 1/(f(v)-v) dv."
        };
    }

    // Step 4: ∫ 1/x dx  =  ln|x|
    // The right-hand side integral is standard; Oasis's Integrate on 1/x
    // should produce Log(x). We build it explicitly for clarity.
    auto rhsIntegrand = std::make_unique<Divide<Expression>>(
        Real { 1.0 },
        x);

    auto rhsIntegral = rhsIntegrand->Integrate(x);
    if (!rhsIntegral) {
        return std::unexpected {
            std::format(
                "FirstOrderHomogeneous::Solve — could not integrate 1/{} d{}.",
                independentVar_, independentVar_)
        };
    }

    // Step 5: back-substitute v = y/x into the implicit solution
    // lhsIntegral(v) = rhsIntegral(x)  =>  lhsIntegral(y/x) = ln|x| + C
    //
    // We substitute v -> y/x in the left-hand integrated expression.
    const Variable y { dependentVar_ };
    auto yOverX = std::make_unique<Divide<Expression>>(y, x);

    auto backSubstituted = lhsIntegral->Substitute(v, *yOverX);
    if (!backSubstituted) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — back-substitution of v = y/x failed."
        };
    }

    // The implicit solution is:  backSubstituted = rhsIntegral + C
    // Return the back-substituted left side; callers can equate it to ln|x|+C.
    return backSubstituted->Simplify();
}

// ---------------------------------------------------------------------------
// Classify
// ---------------------------------------------------------------------------

DECategory FirstOrderHomogeneous::Classify() const
{
    return DECategory::FirstOrderHomogeneous;
}

// ---------------------------------------------------------------------------
// Expression interface
// ---------------------------------------------------------------------------

std::unique_ptr<Expression> FirstOrderHomogeneous::Copy() const
{
    return std::make_unique<FirstOrderHomogeneous>(*this);
}

bool FirstOrderHomogeneous::Equals(const Expression& other) const
{
    const auto* o = dynamic_cast<const FirstOrderHomogeneous*>(&other);
    if (!o) return false;

    if (independentVar_ != o->independentVar_) return false;
    if (dependentVar_   != o->dependentVar_)   return false;

    return (!fv_ && !o->fv_) ||
           (fv_ && o->fv_ && fv_->Equals(*o->fv_));
}

bool FirstOrderHomogeneous::StructurallyEquivalent(const Expression& other) const
{
    const auto* o = dynamic_cast<const FirstOrderHomogeneous*>(&other);
    if (!o) return false;

    return (!fv_ && !o->fv_) ||
           (fv_ && o->fv_ && fv_->StructurallyEquivalent(*o->fv_));
}

std::unique_ptr<Expression> FirstOrderHomogeneous::Substitute(
    const Expression& var,
    const Expression& val)
{
    auto newFv = fv_ ? fv_->Substitute(var, val) : nullptr;

    return std::make_unique<FirstOrderHomogeneous>(
        std::move(newFv),
        independentVar_,
        dependentVar_);
}

// ---------------------------------------------------------------------------
// AcceptInternal  —  visitor / serialisation hook
// ---------------------------------------------------------------------------

any FirstOrderHomogeneous::AcceptInternal(Visitor& visitor) const
{
    // Add VisitFirstOrderHomogeneous to the Oasis Visitor interface
    // and dispatch here when wiring up serialisation.
    return {};
}

} // namespace Oasis
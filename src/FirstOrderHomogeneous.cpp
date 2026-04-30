//
// Created by Adelina Jiang on 4/10/2026.
//
#include "Oasis/FirstOrderHomogeneous.hpp"

#include "Oasis/Divide.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
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
          nullptr,
          nullptr,
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
// Solve  —  substitution v = y/x
// ---------------------------------------------------------------------------

std::expected<std::unique_ptr<Expression>, std::string>
FirstOrderHomogeneous::Solve() const
{
    if (!fv_) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — f(v) is null."
        };
    }

    const Variable v { "v" };
    const Variable x { independentVar_ };

    auto fvMinusV = Subtract<Expression> { *fv_, v }.Simplify();
    if (!fvMinusV) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — could not simplify f(v) - v."
        };
    }

    auto lhsIntegrand = std::make_unique<Divide<Expression>>(
        Real { 1.0 },
        *fvMinusV);

    auto lhsIntegral = lhsIntegrand->Integrate(v);
    if (!lhsIntegral) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — could not integrate 1/(f(v)-v) dv."
        };
    }

    auto rhsIntegrand = std::make_unique<Divide<Expression>>(
        Real { 1.0 },
        x);

    auto rhsIntegral = rhsIntegrand->Integrate(x);
    if (!rhsIntegral) {
        return std::unexpected {
            std::format("FirstOrderHomogeneous::Solve — could not integrate 1/{} d{}.",
                independentVar_, independentVar_)
        };
    }

    const Variable y { dependentVar_ };
    auto yOverX = std::make_unique<Divide<Expression>>(y, x);

    auto backSubstituted = lhsIntegral->Substitute(v, *yOverX);
    if (!backSubstituted) {
        return std::unexpected {
            "FirstOrderHomogeneous::Solve — back-substitution of v = y/x failed."
        };
    }

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
// Verify
// ---------------------------------------------------------------------------

bool FirstOrderHomogeneous::Verify(const Expression& solution) const
{
    if (!fv_) return false;

    const Variable x { independentVar_ };
    const Variable y { dependentVar_ };

    auto dydx = solution.Differentiate(x);
    if (!dydx) return false;

    auto yOverX = Divide<Expression> { solution, x };
    auto fOfYOverX = fv_->Substitute(Variable { "v" }, yOverX);
    if (!fOfYOverX) return false;

    auto lhs = dydx->Simplify();
    auto rhs = fOfYOverX->Simplify();
    if (!lhs || !rhs) return false;

    auto diff = Subtract<Expression> { *lhs, *rhs }.Simplify();
    if (!diff) return false;

    if (auto result = RecursiveCast<Real>(*diff); result != nullptr) {
        return result->GetValue() == 0.0;
    }

    return lhs->Equals(*rhs);
}

// ---------------------------------------------------------------------------
// ToString
// ---------------------------------------------------------------------------

std::string FirstOrderHomogeneous::ToString() const
{
    return std::format("d{}/d{} = f({}/{})",
        dependentVar_, independentVar_,
        dependentVar_, independentVar_);
}

// ---------------------------------------------------------------------------
// AcceptInternal
// ---------------------------------------------------------------------------

any FirstOrderHomogeneous::AcceptInternal(Visitor& visitor) const
{
    return {};
}

} // namespace Oasis
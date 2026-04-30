//
// Created by Adelina Jiang on 4/29/2026.
//


#include "Oasis/FirstOrderSeparable.hpp"

#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

#include <format>

#include "Oasis/FirstOrderSeparable.hpp"

#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

#include <format>

namespace Oasis {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

FirstOrderSeparable::FirstOrderSeparable(
    std::unique_ptr<Expression> gy,
    std::unique_ptr<Expression> fx,
    std::string independentVar,
    std::string dependentVar)
    : DifferentialEquation(
          nullptr,
          nullptr,
          std::move(independentVar),
          std::move(dependentVar))
    , gy_(std::move(gy))
    , fx_(std::move(fx))
{
    category_ = DECategory::FirstOrderSeparable;
}

// ---------------------------------------------------------------------------
// Copy constructor / copy-assignment
// ---------------------------------------------------------------------------

FirstOrderSeparable::FirstOrderSeparable(const FirstOrderSeparable& other)
    : DifferentialEquation(other)
    , gy_(other.gy_ ? other.gy_->Copy() : nullptr)
    , fx_(other.fx_ ? other.fx_->Copy() : nullptr)
{
}

FirstOrderSeparable& FirstOrderSeparable::operator=(const FirstOrderSeparable& other)
{
    if (this == &other) {
        return *this;
    }

    DifferentialEquation::operator=(other);
    gy_ = other.gy_ ? other.gy_->Copy() : nullptr;
    fx_ = other.fx_ ? other.fx_->Copy() : nullptr;

    return *this;
}

// ---------------------------------------------------------------------------
// Solve  —  integrate both sides independently
// ---------------------------------------------------------------------------

std::expected<std::unique_ptr<Expression>, std::string>
FirstOrderSeparable::Solve() const
{
    if (!gy_ || !fx_) {
        return std::unexpected {
            "FirstOrderSeparable::Solve — g(y) or f(x) is null."
        };
    }

    const Variable x { independentVar_ };
    const Variable y { dependentVar_ };

    auto integralGy = gy_->Integrate(y);
    if (!integralGy) {
        return std::unexpected {
            std::format("FirstOrderSeparable::Solve — could not integrate g({}) d{}.",
                dependentVar_, dependentVar_)
        };
    }

    auto integralFx = fx_->Integrate(x);
    if (!integralFx) {
        return std::unexpected {
            std::format("FirstOrderSeparable::Solve — could not integrate f({}) d{}.",
                independentVar_, independentVar_)
        };
    }

    auto implicitSolution = Subtract<Expression> {
        *integralGy,
        *integralFx
    }.Simplify();

    if (!implicitSolution) {
        return std::unexpected {
            "FirstOrderSeparable::Solve — could not simplify implicit solution."
        };
    }

    return implicitSolution;
}

// ---------------------------------------------------------------------------
// Classify
// ---------------------------------------------------------------------------

DECategory FirstOrderSeparable::Classify() const
{
    return DECategory::FirstOrderSeparable;
}

// ---------------------------------------------------------------------------
// Expression interface
// ---------------------------------------------------------------------------

std::unique_ptr<Expression> FirstOrderSeparable::Copy() const
{
    return std::make_unique<FirstOrderSeparable>(*this);
}

bool FirstOrderSeparable::Equals(const Expression& other) const
{
    const auto* o = dynamic_cast<const FirstOrderSeparable*>(&other);
    if (!o) return false;

    if (independentVar_ != o->independentVar_) return false;
    if (dependentVar_   != o->dependentVar_)   return false;

    const bool gyEqual =
        (!gy_ && !o->gy_) ||
        (gy_ && o->gy_ && gy_->Equals(*o->gy_));

    const bool fxEqual =
        (!fx_ && !o->fx_) ||
        (fx_ && o->fx_ && fx_->Equals(*o->fx_));

    return gyEqual && fxEqual;
}

bool FirstOrderSeparable::StructurallyEquivalent(const Expression& other) const
{
    const auto* o = dynamic_cast<const FirstOrderSeparable*>(&other);
    if (!o) return false;

    const bool gyEquiv =
        (!gy_ && !o->gy_) ||
        (gy_ && o->gy_ && gy_->StructurallyEquivalent(*o->gy_));

    const bool fxEquiv =
        (!fx_ && !o->fx_) ||
        (fx_ && o->fx_ && fx_->StructurallyEquivalent(*o->fx_));

    return gyEquiv && fxEquiv;
}

std::unique_ptr<Expression> FirstOrderSeparable::Substitute(
    const Expression& var,
    const Expression& val)
{
    auto newGy = gy_ ? gy_->Substitute(var, val) : nullptr;
    auto newFx = fx_ ? fx_->Substitute(var, val) : nullptr;

    return std::make_unique<FirstOrderSeparable>(
        std::move(newGy),
        std::move(newFx),
        independentVar_,
        dependentVar_);
}

// ---------------------------------------------------------------------------
// Verify
// ---------------------------------------------------------------------------

bool FirstOrderSeparable::Verify(const Expression& solution) const
{
    if (!gy_ || !fx_) return false;

    const Variable x { independentVar_ };
    const Variable y { dependentVar_ };

    auto dydx = solution.Differentiate(x);
    if (!dydx) return false;

    auto gSolution = gy_->Substitute(y, solution);
    if (!gSolution) return false;

    auto lhs = Multiply<Expression> { *gSolution, *dydx }.Simplify();
    if (!lhs) return false;

    auto rhs = fx_->Simplify();
    if (!rhs) return false;

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

std::string FirstOrderSeparable::ToString() const
{
    return std::format("g({}) d{} = f({}) d{}",
        dependentVar_, dependentVar_,
        independentVar_, independentVar_);
}

// ---------------------------------------------------------------------------
// AcceptInternal
// ---------------------------------------------------------------------------

any FirstOrderSeparable::AcceptInternal(Visitor& visitor) const
{
    return {};
}

} // namespace Oasis

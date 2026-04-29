//
// Created by Adelina Jiang on 4/29/2026.
//


#include "Oasis/FirstOrderSeparable.hpp"

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
          nullptr, // lhs — structure lives in gy_/fx_
          nullptr, // rhs
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
//
//   Given:  g(y) dy = f(x) dx
//
//   Step 1: ∫ g(y) dy
//   Step 2: ∫ f(x) dx
//   Step 3: implicit solution is  ∫g(y)dy - ∫f(x)dx = C
//           return the left-hand side; caller equates it to C.
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

    // Step 1: ∫ g(y) dy
    auto integralGy = gy_->Integrate(y);
    if (!integralGy) {
        return std::unexpected {
            std::format(
                "FirstOrderSeparable::Solve — could not integrate g({}) d{}.",
                dependentVar_, dependentVar_)
        };
    }

    // Step 2: ∫ f(x) dx
    auto integralFx = fx_->Integrate(x);
    if (!integralFx) {
        return std::unexpected {
            std::format(
                "FirstOrderSeparable::Solve — could not integrate f({}) d{}.",
                independentVar_, independentVar_)
        };
    }

    // Step 3: implicit solution  ∫g(y)dy - ∫f(x)dx = C
    // Return the left-hand side simplified; caller equates to constant C.
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
// AcceptInternal  —  visitor / serialisation hook
// ---------------------------------------------------------------------------

any FirstOrderSeparable::AcceptInternal(Visitor& visitor) const
{
    // Add VisitFirstOrderSeparable to the Oasis Visitor interface
    // and dispatch here when wiring up serialisation.
    return {};
}

} // namespace Oasis
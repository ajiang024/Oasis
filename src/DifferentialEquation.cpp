//
// Created by Adelina Jiang on 2/3/2026.
//

#include "Oasis/DifferentialEquation.hpp"

#include <format>

namespace Oasis {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

DifferentialEquation::DifferentialEquation(
    std::unique_ptr<Expression> lhs,
    std::unique_ptr<Expression> rhs,
    std::string independentVar,
    std::string dependentVar)
    : lhs_(std::move(lhs))
    , rhs_(std::move(rhs))
    , independentVar_(std::move(independentVar))
    , dependentVar_(std::move(dependentVar))
    , category_(DECategory::Unclassified)
{
}

// ---------------------------------------------------------------------------
// Copy constructor / copy-assignment
// ---------------------------------------------------------------------------

DifferentialEquation::DifferentialEquation(const DifferentialEquation& other)
    : lhs_(other.lhs_ ? other.lhs_->Copy() : nullptr)
    , rhs_(other.rhs_ ? other.rhs_->Copy() : nullptr)
    , independentVar_(other.independentVar_)
    , dependentVar_(other.dependentVar_)
    , category_(other.category_)
{
}

DifferentialEquation& DifferentialEquation::operator=(const DifferentialEquation& other)
{
    if (this == &other) {
        return *this;
    }

    lhs_            = other.lhs_ ? other.lhs_->Copy() : nullptr;
    rhs_            = other.rhs_ ? other.rhs_->Copy() : nullptr;
    independentVar_ = other.independentVar_;
    dependentVar_   = other.dependentVar_;
    category_       = other.category_;

    return *this;
}

// ---------------------------------------------------------------------------
// Expression interface
// ---------------------------------------------------------------------------

std::unique_ptr<Expression> DifferentialEquation::Copy() const
{
    // DifferentialEquation is abstract (Solve() and Classify() are pure virtual).
    // Each concrete subclass must override Copy() with:
    //   return std::make_unique<DerivedType>(*this);
    return nullptr;
}

bool DifferentialEquation::Equals(const Expression& other) const
{
    const auto* otherDE = dynamic_cast<const DifferentialEquation*>(&other);
    if (!otherDE) return false;

    if (independentVar_ != otherDE->independentVar_) return false;
    if (dependentVar_   != otherDE->dependentVar_)   return false;

    const bool lhsEqual =
        (!lhs_ && !otherDE->lhs_) ||
        (lhs_ && otherDE->lhs_ && lhs_->Equals(*otherDE->lhs_));

    const bool rhsEqual =
        (!rhs_ && !otherDE->rhs_) ||
        (rhs_ && otherDE->rhs_ && rhs_->Equals(*otherDE->rhs_));

    return lhsEqual && rhsEqual;
}

std::string DifferentialEquation::ToString() const
{
    // Represents the DE as "d(dependentVar)/d(independentVar) = f(independentVar)"
    // If Expression gains a stable ToString(), replace the rhs placeholder accordingly.
    const std::string lhsStr = std::format("d{}/d{}", dependentVar_, independentVar_);
    const std::string rhsStr = rhs_ ? std::format("f({})", independentVar_) : "<null>";
    return std::format("{} = {}", lhsStr, rhsStr);
}

// ---------------------------------------------------------------------------
// Verify
// ---------------------------------------------------------------------------

bool DifferentialEquation::Verify(const Expression& solution) const
{
    // Stub – override in concrete subclasses:
    //   1. Differentiate `solution` w.r.t. independentVar_ to get y'.
    //   2. Substitute y and y' into the LHS expression.
    //   3. Simplify and compare against RHS.
    //   4. Return true iff the two sides are symbolically equal.
    return !solution.Equals(*rhs_);  // placeholder — keeps 'solution' and 'this' in use
}

} // namespace Oasis
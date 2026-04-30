//
// Created by Adelina Jiang on 2/3/2026.
//

#include "Oasis/DifferentialEquation.hpp"

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
    return std::format("d{}/d{} = f({}, {})",
        dependentVar_, independentVar_,
        independentVar_, dependentVar_);
}

// ---------------------------------------------------------------------------
// Verify  (base stub — overridden in each subclass)
// ---------------------------------------------------------------------------

bool DifferentialEquation::Verify(const Expression& solution) const
{
    return !solution.Equals(*rhs_);
}

// ---------------------------------------------------------------------------
// ClassifyDE  —  static factory
// ---------------------------------------------------------------------------

DECategory DifferentialEquation::ClassifyDE(
    const Expression& lhs,
    const Expression& rhs,
    const std::string& independentVar,
    const std::string& dependentVar)
{
    const Variable x { independentVar };
    const Variable y { dependentVar };

    // Check FirstOrderSeparable:
    // rhs should not contain y (d(rhs)/dy == 0)
    auto dRhsDy = rhs.Differentiate(y);
    auto dLhsDx = lhs.Differentiate(x);

    bool rhsIndependentOfY = false;
    bool lhsIndependentOfX = false;

    if (dRhsDy) {
        if (auto r = RecursiveCast<Real>(*dRhsDy); r != nullptr) {
            rhsIndependentOfY = (r->GetValue() == 0.0);
        }
    }

    if (dLhsDx) {
        if (auto r = RecursiveCast<Real>(*dLhsDx); r != nullptr) {
            lhsIndependentOfX = (r->GetValue() == 0.0);
        }
    }

    if (rhsIndependentOfY && lhsIndependentOfX) {
        return DECategory::FirstOrderSeparable;
    }

    // Check FirstOrderLinear:
    // rhs is linear in y => d²(rhs)/dy² == 0
    auto dRhsDy2 = dRhsDy ? dRhsDy->Differentiate(y) : nullptr;
    if (dRhsDy2) {
        if (auto r = RecursiveCast<Real>(*dRhsDy2); r != nullptr) {
            if (r->GetValue() == 0.0) {
                return DECategory::FirstOrderLinear;
            }
        }
    }

    // Check FirstOrderHomogeneous:
    // substitute y -> v*x; if x disappears from the result it is degree-0 homogeneous
    const Variable v { "v" };
    auto vx = Multiply<Expression> { v, x };
    auto substituted = rhs.Substitute(y, vx);
    if (substituted) {
        auto simplified = substituted->Simplify();
        if (simplified) {
            auto dSimplDx = simplified->Differentiate(x);
            if (dSimplDx) {
                if (auto r = RecursiveCast<Real>(*dSimplDx); r != nullptr) {
                    if (r->GetValue() == 0.0) {
                        return DECategory::FirstOrderHomogeneous;
                    }
                }
            }
        }
    }

    return DECategory::Unclassified;
}

} // namespace Oasis
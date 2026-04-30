//
// Created by Adelina Jiang on 3/27/2026.
//

#include "Oasis/FirstOrderLinear.hpp"

#include "Oasis/Add.hpp"
#include "Oasis/Divide.hpp"
#include "Oasis/EulerNumber.hpp"
#include "Oasis/Exponent.hpp"
#include "Oasis/Integral.hpp"
#include "Oasis/Multiply.hpp"
#include "Oasis/RecursiveCast.hpp"
#include "Oasis/Subtract.hpp"
#include "Oasis/Variable.hpp"

#include <format>

namespace Oasis {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

FirstOrderLinear::FirstOrderLinear(
    std::unique_ptr<Expression> px,
    std::unique_ptr<Expression> qx,
    std::string independentVar,
    std::string dependentVar)
    : DifferentialEquation(
          nullptr,
          nullptr,
          std::move(independentVar),
          std::move(dependentVar))
    , px_(std::move(px))
    , qx_(std::move(qx))
{
    category_ = DECategory::FirstOrderLinear;
}

// ---------------------------------------------------------------------------
// Copy constructor / copy-assignment
// ---------------------------------------------------------------------------

FirstOrderLinear::FirstOrderLinear(const FirstOrderLinear& other)
    : DifferentialEquation(other)
    , px_(other.px_ ? other.px_->Copy() : nullptr)
    , qx_(other.qx_ ? other.qx_->Copy() : nullptr)
{
}

FirstOrderLinear& FirstOrderLinear::operator=(const FirstOrderLinear& other)
{
    if (this == &other) {
        return *this;
    }

    DifferentialEquation::operator=(other);
    px_ = other.px_ ? other.px_->Copy() : nullptr;
    qx_ = other.qx_ ? other.qx_->Copy() : nullptr;

    return *this;
}

// ---------------------------------------------------------------------------
// Solve  —  integrating factor method
// ---------------------------------------------------------------------------

std::expected<std::unique_ptr<Expression>, std::string>
FirstOrderLinear::Solve() const
{
    if (!px_ || !qx_) {
        return std::unexpected { "FirstOrderLinear::Solve — P(x) or Q(x) is null." };
    }

    const Variable x { independentVar_ };

    auto integralOfP = px_->Integrate(x);
    if (!integralOfP) {
        return std::unexpected {
            std::format("FirstOrderLinear::Solve — could not integrate P({}).", independentVar_)
        };
    }

    auto mu = std::make_unique<Exponent<Expression>>(
        EulerNumber {},
        *integralOfP);

    auto muTimesQ = std::make_unique<Multiply<Expression>>(
        *mu,
        *qx_);

    auto integralOfMuQ = muTimesQ->Integrate(x);
    if (!integralOfMuQ) {
        return std::unexpected {
            std::format("FirstOrderLinear::Solve — could not integrate μ·Q({}).", independentVar_)
        };
    }

    auto solution = std::make_unique<Multiply<Expression>>(
        Divide<Expression> { Real { 1.0 }, *mu },
        *integralOfMuQ);

    return solution->Simplify();
}

// ---------------------------------------------------------------------------
// Classify
// ---------------------------------------------------------------------------

DECategory FirstOrderLinear::Classify() const
{
    return DECategory::FirstOrderLinear;
}

// ---------------------------------------------------------------------------
// Expression interface
// ---------------------------------------------------------------------------

std::unique_ptr<Expression> FirstOrderLinear::Copy() const
{
    return std::make_unique<FirstOrderLinear>(*this);
}

bool FirstOrderLinear::Equals(const Expression& other) const
{
    const auto* o = dynamic_cast<const FirstOrderLinear*>(&other);
    if (!o) return false;

    if (independentVar_ != o->independentVar_) return false;
    if (dependentVar_   != o->dependentVar_)   return false;

    const bool pxEqual =
        (!px_ && !o->px_) ||
        (px_ && o->px_ && px_->Equals(*o->px_));

    const bool qxEqual =
        (!qx_ && !o->qx_) ||
        (qx_ && o->qx_ && qx_->Equals(*o->qx_));

    return pxEqual && qxEqual;
}

bool FirstOrderLinear::StructurallyEquivalent(const Expression& other) const
{
    const auto* o = dynamic_cast<const FirstOrderLinear*>(&other);
    if (!o) return false;

    const bool pxEquiv =
        (!px_ && !o->px_) ||
        (px_ && o->px_ && px_->StructurallyEquivalent(*o->px_));

    const bool qxEquiv =
        (!qx_ && !o->qx_) ||
        (qx_ && o->qx_ && qx_->StructurallyEquivalent(*o->qx_));

    return pxEquiv && qxEquiv;
}

std::unique_ptr<Expression> FirstOrderLinear::Substitute(
    const Expression& var,
    const Expression& val)
{
    auto newPx = px_ ? px_->Substitute(var, val) : nullptr;
    auto newQx = qx_ ? qx_->Substitute(var, val) : nullptr;

    return std::make_unique<FirstOrderLinear>(
        std::move(newPx),
        std::move(newQx),
        independentVar_,
        dependentVar_);
}

// ---------------------------------------------------------------------------
// Verify
// ---------------------------------------------------------------------------

bool FirstOrderLinear::Verify(const Expression& solution) const
{
    if (!px_ || !qx_) return false;

    const Variable x { independentVar_ };

    auto dydx = solution.Differentiate(x);
    if (!dydx) return false;

    auto py = Multiply<Expression> { *px_, solution }.Simplify();
    if (!py) return false;

    auto lhs = Add<Expression> { *dydx, *py }.Simplify();
    if (!lhs) return false;

    auto rhs = qx_->Simplify();
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

std::string FirstOrderLinear::ToString() const
{
    return std::format("d{}/d{} + P({})*{} = Q({})",
        dependentVar_, independentVar_,
        independentVar_, dependentVar_,
        independentVar_);
}

// ---------------------------------------------------------------------------
// AcceptInternal
// ---------------------------------------------------------------------------

any FirstOrderLinear::AcceptInternal(Visitor& visitor) const
{
    return {};
}

} // namespace Oasis

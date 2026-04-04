//
// Created by Adelina Jiang on 4/3/2026.
//

#ifndef OASIS_FIRSTORDERHOMOGENEOUS_HPP
#define OASIS_FIRSTORDERHOMOGENEOUS_HPP


#include "Oasis/DifferentialEquation.hpp"

namespace Oasis {

/**
 * Represents a first-order homogeneous ordinary differential equation of the form:
 *
 *     dy/dx = f(y/x)
 *
 * Solved via the substitution v = y/x  (so y = vx, dy/dx = v + x·dv/dx),
 * which reduces the equation to a separable DE in v and x:
 *
 *     x·dv/dx = f(v) - v
 *     =>  dv / (f(v) - v)  =  dx / x
 *     =>  ∫ dv/(f(v)-v)    =  ∫ dx/x  =  ln|x| + C
 *
 * After integrating, back-substitute v = y/x to recover y(x).
 */
class FirstOrderHomogeneous : public DifferentialEquation {
public:
    /**
     * @param fv             The expression f(v) where v represents y/x.
     *                       Pass the right-hand side with y/x already substituted as v.
     * @param independentVar Name of the independent variable (default "x").
     * @param dependentVar   Name of the dependent variable   (default "y").
     */
    FirstOrderHomogeneous(
        std::unique_ptr<Expression> fv,
        std::string independentVar = "x",
        std::string dependentVar   = "y");

    // Rule-of-five
    FirstOrderHomogeneous(const FirstOrderHomogeneous&);
    FirstOrderHomogeneous& operator=(const FirstOrderHomogeneous&);
    FirstOrderHomogeneous(FirstOrderHomogeneous&&) noexcept            = default;
    FirstOrderHomogeneous& operator=(FirstOrderHomogeneous&&) noexcept = default;
    ~FirstOrderHomogeneous() override                                  = default;

    // -----------------------------------------------------------------------
    // DifferentialEquation interface
    // -----------------------------------------------------------------------

    /**
     * Solves the DE via the v = y/x substitution.
     *
     * Steps:
     *   1. Let v = y/x, so f(v) is already stored in fv_.
     *   2. Form the separable equation: dv/(f(v) - v) = dx/x.
     *   3. Integrate both sides.
     *   4. Back-substitute v = y/x.
     *
     * Returns y(x) as an Expression, or an error string on failure.
     */
    [[nodiscard]] std::expected<std::unique_ptr<Expression>, std::string>
    Solve() const override;

    /**
     * Always returns DECategory::FirstOrderHomogeneous.
     */
    [[nodiscard]] DECategory Classify() const override;

    // -----------------------------------------------------------------------
    // Expression interface
    // -----------------------------------------------------------------------

    [[nodiscard]] std::unique_ptr<Expression> Copy() const override;
    [[nodiscard]] bool Equals(const Expression& other) const override;
    [[nodiscard]] bool StructurallyEquivalent(const Expression& other) const override;

    std::unique_ptr<Expression> Substitute(
        const Expression& var,
        const Expression& val) override;

    // -----------------------------------------------------------------------
    // Accessor
    // -----------------------------------------------------------------------

    /**
     * Returns f(v) — the right-hand side with the substitution v = y/x already applied.
     */
    [[nodiscard]] const Expression& GetFv() const { return *fv_; }

protected:
    any AcceptInternal(Visitor& visitor) const override;

private:
    std::unique_ptr<Expression> fv_; ///< f(v) with v = y/x
};

}



#endif //OASIS_FIRSTORDERHOMOGENEOUS_HPP
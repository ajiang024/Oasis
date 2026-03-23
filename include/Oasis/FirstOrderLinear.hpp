//
// Created by Adelina Jiang on 3/22/2026.
//

#ifndef OASIS_FIRSTORDERLINEAR_HPP
#define OASIS_FIRSTORDERLINEAR_HPP


#include "Oasis/DifferentialEquation.hpp"

namespace Oasis {

/**
 * Represents a first-order linear ordinary differential equation of the form:
 *
 *     dy/dx + P(x)·y = Q(x)
 *
 * Solved via the integrating factor method:
 *     μ(x) = e^(∫P(x)dx)
 *     y    = (1/μ(x)) · (∫μ(x)·Q(x)dx + C)
 */
class FirstOrderLinear : public DifferentialEquation {
public:
    /**
     * @param px             The expression P(x) (coefficient of y).
     * @param qx             The expression Q(x) (right-hand side).
     * @param independentVar Name of the independent variable (default "x").
     * @param dependentVar   Name of the dependent variable   (default "y").
     */
    FirstOrderLinear(
        std::unique_ptr<Expression> px,
        std::unique_ptr<Expression> qx,
        std::string independentVar = "x",
        std::string dependentVar   = "y");

    // Rule-of-five
    FirstOrderLinear(const FirstOrderLinear&);
    FirstOrderLinear& operator=(const FirstOrderLinear&);
    FirstOrderLinear(FirstOrderLinear&&) noexcept            = default;
    FirstOrderLinear& operator=(FirstOrderLinear&&) noexcept = default;
    ~FirstOrderLinear() override                             = default;

    // -----------------------------------------------------------------------
    // DifferentialEquation interface
    // -----------------------------------------------------------------------

    /**
     * Solves the DE using the integrating factor method.
     * Returns y(x) = (1/μ)·(∫μ·Q dx + C) as an Expression,
     * or an error string if no closed-form solution can be found.
     */
    [[nodiscard]] std::expected<std::unique_ptr<Expression>, std::string>
    Solve() const override;

    /**
     * Always returns DECategory::FirstOrderLinear.
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
    // Accessors
    // -----------------------------------------------------------------------

    [[nodiscard]] const Expression& GetPx() const { return *px_; }
    [[nodiscard]] const Expression& GetQx() const { return *qx_; }

protected:
    any AcceptInternal(Visitor& visitor) const override;

private:
    std::unique_ptr<Expression> px_; ///< P(x) — coefficient of y
    std::unique_ptr<Expression> qx_; ///< Q(x) — right-hand side forcing term
};

} // namespace Oasis

#endif // OASIS_FIRSTORDERLINEAR_HPP
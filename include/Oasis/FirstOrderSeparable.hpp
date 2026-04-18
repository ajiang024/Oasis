//
// Created by Adelina Jiang on 4/17/2026.
//

#ifndef OASIS_FIRSTORDERSEPARABLE_HPP
#define OASIS_FIRSTORDERSEPARABLE_HPP

#include "Oasis/DifferentialEquation.hpp"

namespace Oasis {

/**
 * Represents a first-order separable ordinary differential equation of the form: g(y) · dy/dx = f(x)
 * Which is rewritten as: g(y) dy = f(x) dx
 * And solved by integrating both sides independently: ∫ g(y) dy = ∫ f(x) dx + C
 */
class FirstOrderSeparable : public DifferentialEquation {
public:
    /**
     * @param gx             The expression g(y) — the y-dependent factor on the left.
     * @param fx             The expression f(x) — the x-dependent factor on the right.
     * @param independentVar Name of the independent variable (default "x").
     * @param dependentVar   Name of the dependent variable   (default "y").
     */
    FirstOrderSeparable(
        std::unique_ptr<Expression> gy,
        std::unique_ptr<Expression> fx,
        std::string independentVar = "x",
        std::string dependentVar   = "y");

    // Rule-of-five
    FirstOrderSeparable(const FirstOrderSeparable&);
    FirstOrderSeparable& operator=(const FirstOrderSeparable&);
    FirstOrderSeparable(FirstOrderSeparable&&) noexcept            = default;
    FirstOrderSeparable& operator=(FirstOrderSeparable&&) noexcept = default;
    ~FirstOrderSeparable() override                                = default;

    /**
     * Solves the DE by integrating both sides independently.
     * Steps:
     *   1. Compute ∫ g(y) dy.
     *   2. Compute ∫ f(x) dx.
     *   3. Return the implicit solution ∫g(y)dy = ∫f(x)dx as the left-hand side.
     * Returns the integrated left-hand side as an Expression, or an error
     * string if either integral cannot be evaluated in closed form.
     */
    [[nodiscard]] std::expected<std::unique_ptr<Expression>, std::string>
    Solve() const override;


    [[nodiscard]] DECategory Classify() const override;

    // Expression interface


    [[nodiscard]] std::unique_ptr<Expression> Copy() const override;
    [[nodiscard]] bool Equals(const Expression& other) const override;
    [[nodiscard]] bool StructurallyEquivalent(const Expression& other) const override;

    std::unique_ptr<Expression> Substitute(
        const Expression& var,
        const Expression& val) override;


    // Accessors


    /** Returns g(y) — the y-dependent factor. */
    [[nodiscard]] const Expression& GetGy() const { return *gy_; }

    /** Returns f(x) — the x-dependent factor. */
    [[nodiscard]] const Expression& GetFx() const { return *fx_; }

protected:
    any AcceptInternal(Visitor& visitor) const override;

private:
    std::unique_ptr<Expression> gy_; ///< g(y) — left-hand side factor in y
    std::unique_ptr<Expression> fx_; ///< f(x) — right-hand side factor in x
};

} // namespace Oasis



#endif //OASIS_FIRSTORDERSEPARABLE_HPP
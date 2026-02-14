//
// Created by Adelina Jiang on 2/3/2026.
//

#ifndef OASIS_DIFFERENTIALEQUATION_H
#define OASIS_DIFFERENTIALEQUATION_H

#include "Expression.hpp"
#include "Variable.hpp"
#include <string>
#include <memory>

namespace Oasis {

    enum class DECategory {
        Unclassified,
        FirstOrderHomogeneous,
        FirstOrderLinear,
        FirstOrderSeparable
    };

    /**
     * Abstract base class for all differential equations in Oasis.
     */
    class DifferentialEquation : public Expression {
    public:
        /**
        * @param lhs            The left-hand side of the equation (usually dy/dx).
        * @param rhs            The right-hand side (the expression it equals).
        * @param independentVar Name of the independent variable (default "x").
        * @param dependentVar   Name of the dependent variable   (default "y").
        */
        DifferentialEquation(
            std::unique_ptr<Expression> lhs,
            std::unique_ptr<Expression> rhs,
            std::string independentVar = "x",
            std::string dependentVar   = "y"
        );

        // Rule-of-five boilerplate
        DifferentialEquation(const DifferentialEquation&);
        DifferentialEquation& operator=(const DifferentialEquation&);
        DifferentialEquation(DifferentialEquation&&) noexcept            = default;
        DifferentialEquation& operator=(DifferentialEquation&&) noexcept = default;
        ~DifferentialEquation() override                                  = default;

        /**
        * Solve the differential equation and return y(x) as an Expression.
        *
        * Returns an error string when no closed-form solution can be found.
        */
        [[nodiscard]] virtual std::expected<std::unique_ptr<Expression>, std::string>
        Solve() const = 0;

        /**
        * @brief Classify and return the DECategory for this equation.
        */
        [[nodiscard]] virtual DECategory Classify() const = 0;

        // -----------------------------------------------------------------------
        // Expression interface (required by Oasis base class)
        // -----------------------------------------------------------------------

        [[nodiscard]] std::unique_ptr<Expression> Copy() const override;
        [[nodiscard]] bool Equals(const Expression& other) const override;
        [[nodiscard]] std::string ToString() const;

        /**
        * @brief Plug a candidate solution back in and verify it satisfies the DE.
        *
        * @param solution  An Expression representing y as a function of x.
        * @return true if the solution is verified symbolically.
        */
        [[nodiscard]] bool Verify(const Expression& solution) const;

        // -----------------------------------------------------------------------
        // Accessors
        // -----------------------------------------------------------------------

        [[nodiscard]] const Expression&  GetLHS()            const { return *lhs_; }
        [[nodiscard]] const Expression&  GetRHS()            const { return *rhs_; }
        [[nodiscard]] const std::string& GetIndependentVar() const { return independentVar_; }
        [[nodiscard]] const std::string& GetDependentVar()   const { return dependentVar_; }
        [[nodiscard]] DECategory         GetDECategory()       const { return category_; }

        protected:
        std::unique_ptr<Expression> lhs_;
        std::unique_ptr<Expression> rhs_;
        std::string                 independentVar_;
        std::string                 dependentVar_;
        DECategory                  category_ { DECategory::Unclassified };
    };
};

#endif //OASIS_DIFFERENTIALEQUATION_H
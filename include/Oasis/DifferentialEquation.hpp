//
// Created by Adelina Jiang on 2/3/2026.
//

#ifndef OASIS_DIFFERENTIALEQUATION_H
#define OASIS_DIFFERENTIALEQUATION_H

#include "Expression.hpp"
#include <string>
#include <memory>

namespace Oasis {

    enum class DEType {
        UNKNOWN,
        FIRST_ORDER_LINEAR,
        FIRST_ORDER_SEPARABLE,
        FIRST_ORDER_HOMOGENEOUS,
        EXACT,
        BERNOULLI
    };

    class DifferentialEquation : public Expression {
    protected:
        std::unique_ptr<Expression> lhs; // Left hand side of eqn
        std::unique_ptr<Expression> rhs; // Right hand side of eqn
        std::string independentVar;
        std::string dependentVar;
        DEType type;

    public:
        DifferentialEquation(
            std::unique_ptr<Expression> lhs,
            std::unique_ptr<Expression> rhs,
            const std::string& independentVar,
            const std::string& dependentVar);

        virtual ~DifferentialEquation() = default;

        // Core methods
        virtual std::unique_ptr<Expression> solve() = 0;
        virtual DEType classify() = 0;
        virtual bool verify(const Expression& solution) const;

        // "Getter" Functions
        const Expression* getLHS() const { return lhs.get(); }
        const Expression* getRHS() const { return rhs.get(); }
        std::string getIndependentVar() const { return independentVar; }
        std::string getDependentVar() const { return dependentVar; }
        DEType getType() const { return type; }

        // Display
        virtual std::string toString() const override;
    };


}

#endif //OASIS_DIFFERENTIALEQUATION_H
//
// Created by Adelina Jiang on 2/3/2026.
//

namespace Oasis {

    enum class DECategory {
        Unclassified,
        FirstOrderHomogeneous,
        FirstOrderLinear,
        FirstOrderSeparable
    };

    class DifferentialEquation : public Expression {
    public:
        DifferentialEquation(
            std::unique_ptr<Expression> lhs,
            std::unique_ptr<Expression> rhs,
            std::string independentVar = "x",
            std::string dependentVar   = "y"
        );

        DifferentialEquation(const DifferentialEquation&);
        DifferentialEquation& operator=(const DifferentialEquation&);
        DifferentialEquation(DifferentialEquation&&) noexcept            = default;
        DifferentialEquation& operator=(DifferentialEquation&&) noexcept = default;
        ~DifferentialEquation() override                                  = default;

        [[nodiscard]] virtual std::expected<std::unique_ptr<Expression>, std::string>
        Solve() const = 0;

        [[nodiscard]] virtual DECategory Classify() const = 0;

        [[nodiscard]] std::unique_ptr<Expression> Copy() const override;
        [[nodiscard]] bool Equals(const Expression& other) const override;
        [[nodiscard]] std::string ToString() const;

        [[nodiscard]] virtual bool Verify(const Expression& solution) const;

        [[nodiscard]] static DECategory ClassifyDE(
            const Expression& lhs,
            const Expression& rhs,
            const std::string& independentVar = "x",
            const std::string& dependentVar   = "y");

        [[nodiscard]] const Expression&  GetLHS()            const { return *lhs_; }
        [[nodiscard]] const Expression&  GetRHS()            const { return *rhs_; }
        [[nodiscard]] const std::string& GetIndependentVar() const { return independentVar_; }
        [[nodiscard]] const std::string& GetDependentVar()   const { return dependentVar_; }
        [[nodiscard]] DECategory         GetDECategory()     const { return category_; }

    protected:
        std::unique_ptr<Expression> lhs_;
        std::unique_ptr<Expression> rhs_;
        std::string                 independentVar_;
        std::string                 dependentVar_;
        DECategory                  category_ { DECategory::Unclassified };
    };

} // namespace Oasis

#endif //OASIS_DIFFERENTIALEQUATION_HPP
//
// Created by Adelina Jiang on 4/17/2026.
//

#ifndef OASIS_FIRSTORDERSEPARABLE_HPP
#define OASIS_FIRSTORDERSEPARABLE_HPP

#include "Oasis/DifferentialEquation.hpp"

namespace Oasis {

    class FirstOrderSeparable : public DifferentialEquation {
    public:
        FirstOrderSeparable(
            std::unique_ptr<Expression> gy,
            std::unique_ptr<Expression> fx,
            std::string independentVar = "x",
            std::string dependentVar   = "y");

        FirstOrderSeparable(const FirstOrderSeparable&);
        FirstOrderSeparable& operator=(const FirstOrderSeparable&);
        FirstOrderSeparable(FirstOrderSeparable&&) noexcept            = default;
        FirstOrderSeparable& operator=(FirstOrderSeparable&&) noexcept = default;
        ~FirstOrderSeparable() override                                = default;

        [[nodiscard]] std::expected<std::unique_ptr<Expression>, std::string>
        Solve() const override;

        [[nodiscard]] DECategory Classify() const override;

        [[nodiscard]] std::unique_ptr<Expression> Copy() const override;
        [[nodiscard]] bool Equals(const Expression& other) const override;
        [[nodiscard]] bool StructurallyEquivalent(const Expression& other) const override;
        [[nodiscard]] bool Verify(const Expression& solution) const override;
        [[nodiscard]] std::string ToString() const override;

        std::unique_ptr<Expression> Substitute(
            const Expression& var,
            const Expression& val) override;

        [[nodiscard]] const Expression& GetGy() const { return *gy_; }
        [[nodiscard]] const Expression& GetFx() const { return *fx_; }

    protected:
        any AcceptInternal(Visitor& visitor) const override;

    private:
        std::unique_ptr<Expression> gy_;
        std::unique_ptr<Expression> fx_;
    };

} // namespace Oasis

#endif // OASIS_FIRSTORDERSEPARABLE_HPP
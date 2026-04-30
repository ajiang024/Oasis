//
// Created by Adelina Jiang on 3/22/2026.
//

#ifndef OASIS_FIRSTORDERLINEAR_HPP
#define OASIS_FIRSTORDERLINEAR_HPP

#include "Oasis/DifferentialEquation.hpp"

namespace Oasis {

    class FirstOrderLinear : public DifferentialEquation {
    public:
        FirstOrderLinear(
            std::unique_ptr<Expression> px,
            std::unique_ptr<Expression> qx,
            std::string independentVar = "x",
            std::string dependentVar   = "y");

        FirstOrderLinear(const FirstOrderLinear&);
        FirstOrderLinear& operator=(const FirstOrderLinear&);
        FirstOrderLinear(FirstOrderLinear&&) noexcept            = default;
        FirstOrderLinear& operator=(FirstOrderLinear&&) noexcept = default;
        ~FirstOrderLinear() override                             = default;

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

        [[nodiscard]] const Expression& GetPx() const { return *px_; }
        [[nodiscard]] const Expression& GetQx() const { return *qx_; }

    protected:
        any AcceptInternal(Visitor& visitor) const override;

    private:
        std::unique_ptr<Expression> px_;
        std::unique_ptr<Expression> qx_;
    };

} // namespace Oasis

#endif // OASIS_FIRSTORDERLINEAR_HPP

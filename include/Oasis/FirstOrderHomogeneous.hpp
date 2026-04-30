//
// Created by Adelina Jiang on 4/3/2026.
//

#ifndef OASIS_FIRSTORDERHOMOGENEOUS_HPP
#define OASIS_FIRSTORDERHOMOGENEOUS_HPP

#include "Oasis/DifferentialEquation.hpp"

namespace Oasis {

    class FirstOrderHomogeneous : public DifferentialEquation {
    public:
        FirstOrderHomogeneous(
            std::unique_ptr<Expression> fv,
            std::string independentVar = "x",
            std::string dependentVar   = "y");

        FirstOrderHomogeneous(const FirstOrderHomogeneous&);
        FirstOrderHomogeneous& operator=(const FirstOrderHomogeneous&);
        FirstOrderHomogeneous(FirstOrderHomogeneous&&) noexcept            = default;
        FirstOrderHomogeneous& operator=(FirstOrderHomogeneous&&) noexcept = default;
        ~FirstOrderHomogeneous() override                                  = default;

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

        [[nodiscard]] const Expression& GetFv() const { return *fv_; }

    protected:
        any AcceptInternal(Visitor& visitor) const override;

    private:
        std::unique_ptr<Expression> fv_;
    };

} // namespace Oasis

#endif // OASIS_FIRSTORDERHOMOGENEOUS_HPP
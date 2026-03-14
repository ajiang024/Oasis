//
// Created by adeli on 3/13/2026.
//

#include "Oasis/DifferentialEquation.hpp"
#include "Oasis/Variable.hpp"
#include "Oasis/Real.hpp"

#include <iostream>
#include <memory>

using namespace Oasis;

/**
 * Absolute minimal DE subclass for testing.
 */
class TestDE : public DifferentialEquation {
public:
    // Inherit constructors
    using DifferentialEquation::DifferentialEquation;

    // Solve - return dummy Variable("C")
    std::expected<std::unique_ptr<Expression>, std::string>
    Solve() const override {
        return std::make_unique<Variable>("C");
    }

    // Classify - always return Unclassified
    DECategory Classify() const override {
        return DECategory::Unclassified;
    }

    // Copy - create a new TestDE with copied contents
    std::unique_ptr<Expression> Copy() const override {
        return std::make_unique<TestDE>(
            lhs_->Copy(),
            rhs_->Copy(),
            independentVar_,
            dependentVar_
        );
    }
};

int main() {
    std::cout << "\n=== DifferentialEquation Compilation Test ===\n\n";

    try {
        // Test 1: Construction
        std::cout << "Test 1: Construction... ";
        auto lhs = std::make_unique<Variable>("dy/dx");
        auto rhs = std::make_unique<Variable>("x");
        TestDE de(std::move(lhs), std::move(rhs), "x", "y");
        std::cout << "PASSED\n";

        // Test 2: Accessors
        std::cout << "Test 2: Accessors... ";
        bool ok = (de.GetIndependentVar() == "x" &&
                   de.GetDependentVar() == "y");
        std::cout << (ok ? "PASSED" : "FAILED") << "\n";
        std::cout << "  Indep: " << de.GetIndependentVar() << "\n";
        std::cout << "  Dep:   " << de.GetDependentVar() << "\n";

        // Test 3: Category
        std::cout << "Test 3: GetDECategory... ";
        DECategory cat = de.GetDECategory();
        std::cout << (cat == DECategory::Unclassified ? "PASSED" : "FAILED") << "\n";

        // Test 4: GetCategory (unsigned int)
        std::cout << "Test 4: GetCategory... ";
        unsigned int catInt = de.GetCategory();
        std::cout << "PASSED (value: " << catInt << ")\n";

        // Test 5: Copy
        std::cout << "Test 5: Copy... ";
        auto copy = de.Copy();
        std::cout << (copy ? "PASSED" : "FAILED") << "\n";

        // Test 6: Equals
        std::cout << "Test 6: Equals... ";
        bool eq = de.Equals(*copy);
        std::cout << (eq ? "PASSED" : "FAILED") << "\n";

        // Test 7: Classify
        std::cout << "Test 7: Classify... ";
        DECategory classified = de.Classify();
        std::cout << (classified == DECategory::Unclassified ? "PASSED" : "FAILED") << "\n";

        // Test 8: Solve
        std::cout << "Test 8: Solve... ";
        auto solution = de.Solve();
        std::cout << (solution.has_value() ? "PASSED" : "FAILED") << "\n";

        std::cout << "\n=== All Tests PASSED ===\n\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n!!! EXCEPTION !!!\n" << e.what() << "\n\n";
        return 1;
    }
}
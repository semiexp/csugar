#pragma once

#include <memory>

#include "csp/csp.h"
#include "csp/expr.h"
#include "icsp/icsp.h"
#include "icsp/linear_sum.h"
#include "icsp/linear_literal.h"

namespace csugar {

class Converter {
public:
    Converter(ICSP &icsp) : icsp_(icsp) {}

    void Convert(std::shared_ptr<Expr> expr) {
        ConvertConstraint(expr);
    }

private:
    void ConvertConstraint(std::shared_ptr<Expr> expr);
    std::vector<Clause> ConvertConstraint(std::shared_ptr<Expr> expr, bool negative);
    std::shared_ptr<Expr> ConvertLogical(std::shared_ptr<Expr> expr, bool negative, std::vector<Clause> &clauses);
    std::vector<Clause> ConvertDisj(std::shared_ptr<Expr> expr, bool negative);
    std::shared_ptr<Expr> ConvertComparison(std::shared_ptr<Expr> expr, bool negative, std::vector<Clause> &clauses);
    std::vector<Clause> ConvertComparison(std::shared_ptr<Expr> x, std::shared_ptr<Expr> y, LinearLiteralOp op);
    LinearSum ConvertFormula(std::shared_ptr<Expr> expr);
    LinearSum ReduceArity(const LinearSum &e, LinearLiteralOp op);
    LinearSum SimplifyLinearExpression(const LinearSum& e, LinearLiteralOp op, bool first);

    std::shared_ptr<IntVar> GetEquivalence(std::shared_ptr<Expr> x) {
        // TODO
        return std::shared_ptr<IntVar>(nullptr);
    }
    void AddEquivalence(std::shared_ptr<IntVar> v, std::shared_ptr<Expr> x) {
        // TODO
    }

    ICSP& icsp_;
};

}

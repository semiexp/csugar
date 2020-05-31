#pragma once

#include <memory>

#include "csp/csp.h"
#include "csp/expr.h"
#include "csp/var.h"
#include "icsp/icsp.h"
#include "icsp/var.h"
#include "icsp/linear_sum.h"
#include "icsp/linear_literal.h"
#include "common/config.h"

namespace csugar {

class Converter {
public:
    Converter(CSP& csp, ICSP &icsp) : csp_(csp), icsp_(icsp) {}

    void Convert(bool incremental = false);
    Config GetConfig() const { return config_; }
    void SetCofig(const Config& config) { config_ = config; }

    std::shared_ptr<ICSPBoolVar> ConvertBoolVar(const CSPBoolVar &var) {
        if (0 <= var.id() && var.id() < bool_var_conv_.size()) {
            return bool_var_conv_[var.id()];
        }
        abort(); // TODO
    }
    std::shared_ptr<ICSPIntVar> ConvertIntVar(const CSPIntVar &var) {
        if (0 <= var.id() && var.id() < int_var_conv_.size()) {
            return int_var_conv_[var.id()];
        }
        abort(); // TODO
    }

private:
    void ConvertConstraint(std::shared_ptr<Expr> expr);
    std::vector<Clause> ConvertConstraint(std::shared_ptr<Expr> expr, bool negative, bool top_level = false);
    std::shared_ptr<Expr> ConvertLogical(std::shared_ptr<Expr> expr, bool negative, std::vector<Clause> &clauses);
    std::vector<Clause> ConvertDisj(std::shared_ptr<Expr> expr, bool negative);
    std::shared_ptr<Expr> ConvertAllDifferent(std::shared_ptr<Expr> expr);
    std::shared_ptr<Literal> ConvertGraphConstraints(std::shared_ptr<Expr> expr);
    std::shared_ptr<Expr> ConvertComparison(std::shared_ptr<Expr> expr, bool negative, std::vector<Clause> &clauses);
    std::vector<Clause> ConvertComparison(std::shared_ptr<Expr> x, std::shared_ptr<Expr> y, LinearLiteralOp op);
    LinearSum ConvertFormula(std::shared_ptr<Expr> expr);
    LinearSum ReduceArity(const LinearSum &e, LinearLiteralOp op);
    LinearSum SimplifyLinearExpression(const LinearSum& e, LinearLiteralOp op, bool first);

    std::shared_ptr<ICSPIntVar> GetEquivalence(std::shared_ptr<Expr> x) {
        // TODO
        for (auto& p : cache_) {
            if (Expr::Equal(x, p.first)) return p.second;
        }
        return std::shared_ptr<ICSPIntVar>(nullptr);
    }
    void AddEquivalence(std::shared_ptr<ICSPIntVar> v, std::shared_ptr<Expr> x) {
        // TODO
        cache_.push_back({x, v});
    }

    CSP& csp_;
    ICSP& icsp_;
    std::vector<std::pair<std::shared_ptr<Expr>, std::shared_ptr<ICSPIntVar>>> cache_;
    std::vector<std::shared_ptr<ICSPBoolVar>> bool_var_conv_;
    std::vector<std::shared_ptr<ICSPIntVar>> int_var_conv_;
    Config config_;
};

}

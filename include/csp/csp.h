#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "csp/expr.h"
#include "csp/var.h"
#include "common/domain.h"

namespace csugar {

class CSP {
public:
    CSP() : converted_exprs_(0), num_bool_vars_(0), converted_bool_vars_(0), converted_int_vars_(0) {}

    const std::vector<std::shared_ptr<Expr>>& Exprs() const {
        return exprs_;
    }
    int NumConvertedExprs() const {
        return converted_exprs_;
    }
    int NumConvertedBoolVars() const {
        return converted_bool_vars_;
    }
    int NumConvertedIntVars() const {
        return converted_int_vars_;
    }
    void SetAllConverted() {
        converted_exprs_ = exprs_.size();
        converted_bool_vars_ = num_bool_vars_;
        converted_int_vars_ = NumIntVars();
    }
    int NumBoolVars() const {
        return num_bool_vars_;
    }
    int NumIntVars() const {
        return int_var_domains_.size();
    }
    const std::unique_ptr<Domain>& GetIntVarDomain(int i) const {
        return int_var_domains_[i];
    }
    void AddExpr(std::shared_ptr<Expr> expr) {
        exprs_.push_back(expr);
    }
    CSPBoolVar MakeBoolVar() {
        return CSPBoolVar(num_bool_vars_++);
    }
    CSPIntVar MakeIntVar(std::unique_ptr<Domain>&& domain) {
        CSPIntVar ret(static_cast<int>(int_var_domains_.size()));
        int_var_domains_.push_back(std::move(domain));
        return ret;
    }

private:
    std::vector<std::shared_ptr<Expr>> exprs_;
    // std::map<std::string, std::shared_ptr<BoolVar>> bool_vars_;
    // std::map<std::string, std::shared_ptr<IntVar>> int_vars_;
    std::vector<std::unique_ptr<Domain>> int_var_domains_;
    int num_bool_vars_;
    int converted_exprs_, converted_bool_vars_, converted_int_vars_;
};

}

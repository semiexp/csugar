#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "csp/expr.h"
#include "common/var.h"

namespace csugar {

class CSP {
public:
    CSP() {}

    const std::vector<std::shared_ptr<Expr>>& Exprs() const {
        return exprs_;
    }
    const std::map<std::string, std::shared_ptr<BoolVar>>& BoolVars() const {
        return bool_vars_;
    }
    const std::map<std::string, std::shared_ptr<IntVar>>& IntVars() const {
        return int_vars_;
    }
    void AddExpr(std::shared_ptr<Expr> expr) {
        exprs_.push_back(expr);
    }
    void AddBoolVar(std::shared_ptr<BoolVar> var) {
        if (bool_vars_.count(var->name()) > 0) {
            // error
        }
        bool_vars_.insert({var->name(), var});
    }
    void AddIntVar(std::shared_ptr<IntVar> var) {
        if (int_vars_.count(var->name()) > 0) {
            // error
        }
        int_vars_.insert({var->name(), var});
    }

private:
    std::vector<std::shared_ptr<Expr>> exprs_;
    std::map<std::string, std::shared_ptr<BoolVar>> bool_vars_;
    std::map<std::string, std::shared_ptr<IntVar>> int_vars_;
};

}

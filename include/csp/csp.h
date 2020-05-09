#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "csp/expr.h"
#include "common/var.h"
#include "common/domain.h"

namespace csugar {

class CSP {
public:
    CSP() : converted_exprs_(0) {}

    const std::vector<std::shared_ptr<Expr>>& Exprs() const {
        return exprs_;
    }
    int NumConvertedExprs() const {
        return converted_exprs_;
    }
    void SetAllExprsConverted() {
        converted_exprs_ = exprs_.size();
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
    bool HasBoolVar(const std::string& name) {
        return bool_vars_.count(name) > 0;
    }
    void AddBoolVar(std::shared_ptr<BoolVar> var) {
        if (bool_vars_.count(var->name()) > 0) {
            // error
        }
        bool_vars_.insert({var->name(), var});
    }
    std::shared_ptr<BoolVar> AddBoolVar(const std::string& name) {
        std::shared_ptr<BoolVar> ret = std::make_shared<BoolVar>(name);
        AddBoolVar(ret);
        return ret;
    }
    bool HasIntVar(const std::string& name) {
        return int_vars_.count(name) > 0;
    }
    void AddIntVar(std::shared_ptr<IntVar> var) {
        if (int_vars_.count(var->name()) > 0) {
            // error
        }
        int_vars_.insert({var->name(), var});
    }
    std::shared_ptr<IntVar> AddIntVar(std::unique_ptr<Domain>&& domain, const std::string& name) {
        std::shared_ptr<IntVar> ret = std::make_shared<IntVar>(std::move(domain), name);
        AddIntVar(ret);
        return ret;
    }

private:
    std::vector<std::shared_ptr<Expr>> exprs_;
    std::map<std::string, std::shared_ptr<BoolVar>> bool_vars_;
    std::map<std::string, std::shared_ptr<IntVar>> int_vars_;
    int converted_exprs_;
};

}

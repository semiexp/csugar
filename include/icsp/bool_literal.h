#pragma once

#include <memory>
#include <string>
#include <set>
#include <algorithm>

#include "icsp/literal.h"
#include "icsp/var.h"

namespace csugar {

class BoolLiteral : public Literal {
public:
    BoolLiteral(std::shared_ptr<ICSPBoolVar> var, bool negative) : var_(var), negative_(negative) {}

    std::shared_ptr<ICSPBoolVar> var() const { return var_; }
    bool negative() const { return negative_; }

    bool IsValid() const override { return false; }
    bool IsSimple() const override { return true; }
    bool IsUnsatisfiable() const override { return false; }

    std::string str() const {
        if (negative_) return std::string("!b") + std::to_string(var_->id());
        else return std::string("b") + std::to_string(var_->id());
    }

    std::set<std::shared_ptr<ICSPIntVar>> IntVars() const override { return {}; }
    std::pair<int, int> GetBound(std::shared_ptr<ICSPIntVar> v) const override {
        return { v->domain()->GetLowerBound(), v->domain()->GetUpperBound() };
    }

private:
    std::shared_ptr<ICSPBoolVar> var_;
    bool negative_;
};

}

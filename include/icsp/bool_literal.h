#pragma once

#include <memory>
#include <string>

#include "icsp/literal.h"
#include "common/var.h"

namespace csugar {

class BoolLiteral : public Literal {
public:
    BoolLiteral(std::shared_ptr<BoolVar> var, bool negative) : var_(var), negative_(negative) {}

    bool IsSimple() const override { return true; }

    std::string str() const {
        if (negative_) return std::string("!") + var_->name();
        else return var_->name();
    }

private:
    std::shared_ptr<BoolVar> var_;
    bool negative_;
};

}

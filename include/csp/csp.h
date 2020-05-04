#pragma once

#include <vector>
#include <memory>

#include "csp/expr.h"
#include "common/var.h"

namespace csugar {

class CSP
{
public:
private:
    std::vector<std::shared_ptr<Expr>> exprs_;
    std::vector<std::shared_ptr<BoolVar>> bool_vars_;
    std::vector<std::shared_ptr<IntVar>> int_vars_;
};

}

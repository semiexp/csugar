#pragma once

#include <memory>

#include "csp/csp.h"
#include "csp/expr.h"
#include "icsp/icsp.h"

namespace csugar
{

class Converter
{
public:
    void Convert(std::shared_ptr<Expr> expr);

private:
    std::vector<Clause> ConvertConstraint(std::shared_ptr<Expr> expr, bool negative = false);
    std::shared_ptr<Expr> ConvertLogical(std::shared_ptr<Expr> expr, bool negative, std::vector<Clause> &clauses);
    std::vector<Clause> ConvertDisj(std::shared_ptr<Expr> expr, bool negative);
    std::shared_ptr<Expr> ConvertComparison(std::shared_ptr<Expr> expr, bool negative, std::vector<Clause> &clauses);

    ICSP icsp_;
};

}

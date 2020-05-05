#pragma once

#include <memory>
#include <string>

#include "icsp/literal.h"
#include "icsp/linear_sum.h"
#include "common/var.h"

namespace csugar {

enum LinearLiteralOp {
    kLitEq, kLitNe, kLitGe, kLitLe
};

class LinearLiteral : public Literal {
public:
    LinearLiteral(const LinearSum &sum, LinearLiteralOp op) : sum_(sum), op_(op) {}

    bool IsSimple() const override;

    std::string str() const;

private:
    LinearSum sum_;
    LinearLiteralOp op_;
};

}

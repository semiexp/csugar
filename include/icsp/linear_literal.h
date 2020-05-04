#pragma once

#include <memory>

#include "icsp/literal.h"
#include "icsp/linear_sum.h"
#include "common/var.h"

namespace csugar {

enum LinearLiteralOp
{
    kLitEq, kLitNe, kLitGe, kLitLe
};

class LinearLiteral : public Literal
{
public:
    LinearLiteral() {}
private:
    LinearSum sum_;
    LinearLiteralOp op_;
};

}

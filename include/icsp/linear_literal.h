#pragma once

#include <memory>

#include "icsp/literal.h"
#include "common/var.h"

namespace csugar {

enum LinearLiteralOp
{
    kEq, kGe, kLe
};

class LinearLiteral : public Literal
{
public:
    LinearLiteral() {}
private:
    LinearLiteralOp op_;
};

}

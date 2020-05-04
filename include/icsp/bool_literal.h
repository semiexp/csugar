#pragma once

#include <memory>

#include "icsp/literal.h"
#include "common/var.h"

namespace csugar {

class BoolLiteral : public Literal
{
public:
    BoolLiteral() {}
private:
    std::shared_ptr<BoolVar> var_;
    bool is_negative_;
};

}

#pragma once

#include <memory>

#include "icsp/literal.h"
#include "common/var.h"

namespace csugar {

class BoolLiteral : public Literal
{
public:
    BoolLiteral(std::shared_ptr<BoolVar> var, bool negative) : var_(var), negative_(negative) {}
private:
    std::shared_ptr<BoolVar> var_;
    bool negative_;
};

}

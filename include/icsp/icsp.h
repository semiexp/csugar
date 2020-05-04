#pragma once

#include <vector>

#include "icsp/clause.h"

namespace csugar
{

class ICSP
{
public:
private:
    std::vector<Clause> clauses_;
    std::vector<std::shared_ptr<BoolVar>> bool_vars_;
    std::vector<std::shared_ptr<IntVar>> int_vars_;
};

}

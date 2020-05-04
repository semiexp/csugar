#pragma once

#include <vector>
#include <memory>

#include "icsp/literal.h"

namespace csugar
{

class Clause
{
public:
    Clause() {}
    void Add(std::shared_ptr<Literal> p)
    {
        literals_.push_back(p);
    }
private:
    std::vector<std::shared_ptr<Literal>> literals_;
};

}

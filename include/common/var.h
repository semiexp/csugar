#pragma once

#include <set>

namespace csugar {

class BoolVar
{
};

class IntVar
{
public:
    IntVar() {}
private:
    std::set<int> domain_;
};

}

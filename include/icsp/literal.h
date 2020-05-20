#pragma once

#include <string>
#include <set>
#include <memory>
#include <algorithm>

#include "icsp/var.h"

namespace csugar {

class Literal {
public:
    Literal() {}
    virtual ~Literal() {}

    virtual bool IsSimple() const = 0;
    virtual bool IsValid() const = 0;
    virtual bool IsUnsatisfiable() const = 0;

    virtual std::string str() const = 0;

    virtual std::set<std::shared_ptr<ICSPIntVar>> IntVars() const = 0;
    virtual std::pair<int, int> GetBound(std::shared_ptr<ICSPIntVar> v) const = 0;
};

}

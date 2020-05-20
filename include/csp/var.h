#pragma once

#include <set>
#include <string>
#include <memory>

#include "common/domain.h"

namespace csugar {

class CSPBoolVar {
public:
    CSPBoolVar() : id_(-1) {}
    CSPBoolVar(int id) : id_(id) {}

    int id() const { return id_; }
private:
    int id_;
};

class CSPIntVar {
public:
    CSPIntVar() : id_(-1) {}
    CSPIntVar(int id) : id_(id) {}

    int id() const { return id_; }
private:
    int id_;
};

}

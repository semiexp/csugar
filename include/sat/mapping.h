#pragma once

#include <vector>
#include <map>
#include <memory>
#include <algorithm>

#include "common/var.h"
#include "sat/sat.h"
#include "sat/satlit.h"

namespace csugar {

// Class for maintaining the mapping between CSP variables and SAT variables
class Mapping {
public:
    Mapping(SAT& sat) : sat_(sat) {}
    void RegisterMappingBool(std::shared_ptr<const BoolVar> var);
    void RegisterMappingInt(std::shared_ptr<const IntVar> var);

    bool Retrieve(std::shared_ptr<const BoolVar> var, const std::vector<bool> &assignment);
    int Retrieve(std::shared_ptr<const IntVar> var, const std::vector<bool> &assignment);

    SATLit GetCode(std::shared_ptr<const BoolVar> var) { return mapping_bool_[var]; }
    SATLit GetCodeLE(std::shared_ptr<const IntVar> var, int c);
private:
    SAT& sat_;
    std::map<std::shared_ptr<const BoolVar>, int> mapping_bool_;
    std::map<std::shared_ptr<const IntVar>, std::pair<std::vector<int>, int>> mapping_int_;
};

}

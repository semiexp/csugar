#pragma once

#include <vector>
#include <map>
#include <memory>
#include <algorithm>

#include "icsp/var.h"
#include "sat/sat.h"
#include "sat/satlit.h"

namespace csugar {

// Class for maintaining the mapping between CSP variables and SAT variables
class Mapping {
public:
    Mapping(SAT& sat) : sat_(sat) {}
    void RegisterMappingBool(std::shared_ptr<ICSPBoolVar> var);
    void RegisterMappingInt(std::shared_ptr<ICSPIntVar> var);

    bool Retrieve(std::shared_ptr<ICSPBoolVar> var, const std::vector<bool> &assignment);
    int Retrieve(std::shared_ptr<ICSPIntVar> var, const std::vector<bool> &assignment);

    SATLit GetCode(std::shared_ptr<ICSPBoolVar> var) { return mapping_bool_[var]; }
    SATLit GetCodeLE(std::shared_ptr<ICSPIntVar> var, int c);
private:
    SAT& sat_;
    std::map<std::shared_ptr<ICSPBoolVar>, int> mapping_bool_;
    std::map<std::shared_ptr<ICSPIntVar>, std::pair<std::vector<int>, int>> mapping_int_;
};

}

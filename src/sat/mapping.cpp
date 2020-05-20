#include "sat/mapping.h"

#include <algorithm>

namespace csugar {

void Mapping::RegisterMappingBool(std::shared_ptr<ICSPBoolVar> var) {
    if (mapping_bool_.count(var) > 0) {
        // TODO: error
    }
    int id = sat_.NumVariables();
    sat_.AddVariables(1);
    mapping_bool_.insert({var, id});
}
void Mapping::RegisterMappingInt(std::shared_ptr<ICSPIntVar> var) {
    if (mapping_int_.count(var) > 0) {
        // TODO: error
    }
    int id = sat_.NumVariables();
    std::vector<int> domain = var->domain()->Enumerate();
    if (domain.size() == 0) {
        // TODO: error
    }
    mapping_int_.insert({var, {domain, id }});
    sat_.AddVariables((int)domain.size() - 1);
}
bool Mapping::Retrieve(std::shared_ptr<ICSPBoolVar> var, const std::vector<bool> &assignment) {
    int id = mapping_bool_.at(var);
    return assignment[id];
}
int Mapping::Retrieve(std::shared_ptr<ICSPIntVar> var, const std::vector<bool> &assignment) {
    auto& info = mapping_int_.at(var);
    for (int i = 0; i < (int)info.first.size() - 1; ++i) {
        if (assignment[info.second + i]) {
            return info.first[i];
        }
    }
    return info.first.back();
}
SATLit Mapping::GetCodeLE(std::shared_ptr<ICSPIntVar> var, int c) {
    auto& info = mapping_int_[var];
    if (c < info.first[0]) {
        return sat_.False();
    } else if (c >= info.first.back()) {
        return sat_.True();
    }
    int dist = std::distance(info.first.begin(), std::upper_bound(info.first.begin(), info.first.end(), c));
    return SATLit(info.second + dist - 1);
}

}

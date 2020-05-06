#include "sat/mapping.h"

#include <algorithm>

namespace csugar {

void Mapping::RegisterMappingBool(std::shared_ptr<const BoolVar> var) {
    if (mapping_bool_.count(var) > 0) {
        // TODO: error
    }
    int id = sat_.n_variables++;
    mapping_bool_.insert({var, id});
}
void Mapping::RegisterMappingInt(std::shared_ptr<const IntVar> var) {
    if (mapping_int_.count(var) > 0) {
        // TODO: error
    }
    int id = sat_.n_variables;
    std::vector<int> domain = var->domain()->Enumerate();
    if (domain.size() == 0) {
        // TODO: error
    }
    mapping_int_.insert({var, {domain, id }});
    sat_.n_variables += (int)domain.size() - 1;
}
bool Mapping::Retrieve(std::shared_ptr<const BoolVar> var, const std::vector<bool> &assignment) {
    int id = mapping_bool_.at(var);
    return assignment[id];
}
int Mapping::Retrieve(std::shared_ptr<const IntVar> var, const std::vector<bool> &assignment) {
    auto& info = mapping_int_.at(var);
    for (int i = 0; i < (int)info.first.size() - 1; ++i) {
        if (assignment[info.second + i]) {
            return info.first[i];
        }
    }
    return info.first.back();
}
SATLit Mapping::GetCodeLE(std::shared_ptr<const IntVar> var, int c) {
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

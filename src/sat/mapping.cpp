#include "sat/mapping.h"

#include <algorithm>

namespace csugar {

void Mapping::RegisterMappingBool(std::shared_ptr<const BoolVar> var) {
    if (mapping_bool_.count(var) > 0) {
        // TODO: error
    }
    int id = ++sat_.n_variables;
    mapping_bool_.insert({var, id});
}
void Mapping::RegisterMappingInt(std::shared_ptr<const IntVar> var) {
    if (mapping_int_.count(var) > 0) {
        // TODO: error
    }
    int id = sat_.n_variables + 1;
    std::vector<int> domain = var->domain()->Enumerate();
    mapping_int_.insert({var, {domain, id }});
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

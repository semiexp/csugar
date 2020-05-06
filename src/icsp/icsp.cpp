#include "icsp/icsp.h"

#include <string>

#include "csp/csp.h"
#include "common/var.h"
#include "common/domain.h"

namespace csugar {

void ICSP::LoadVars(const CSP& csp) {
    for (auto p : csp.BoolVars()) {
        AddBoolVar(p.second);
    }
    for (auto p : csp.IntVars()) {
        AddIntVar(p.second);
    }
}
void ICSP::AddBoolVar(std::shared_ptr<BoolVar> var) {
    if (bool_var_map_.count(var->name()) > 0) {
        // TODO: error
    }
    bool_vars_.push_back(var);
    bool_var_map_.insert({var->name(), var});
}
void ICSP::AddIntVar(std::shared_ptr<IntVar> var) {
    if (int_var_map_.count(var->name()) > 0) {
        // TODO: error
    }
    int_vars_.push_back(var);
    int_var_map_.insert({var->name(), var});
}
bool ICSP::HasBoolVar(const std::string &name) {
    return bool_var_map_.count(name) > 0;
}
std::shared_ptr<BoolVar> ICSP::GetBoolVar(const std::string& name) {
    return bool_var_map_[name];
}
bool ICSP::HasIntVar(const std::string &name) {
    return int_var_map_.count(name) > 0;
}
std::shared_ptr<IntVar> ICSP::GetIntVar(const std::string& name) {
    return int_var_map_[name];
}
std::shared_ptr<BoolVar> ICSP::AuxiliaryBoolVar() {
    std::string name = "0_b_" + std::to_string(auxiliary_id_++);
    auto v = std::make_shared<BoolVar>(name);
    AddBoolVar(v);
    return v;
}
std::shared_ptr<IntVar> ICSP::AuxiliaryIntVar(std::unique_ptr<Domain>&& domain) {
    std::string name = "0_i_" + std::to_string(auxiliary_id_++);
    auto v = std::make_shared<IntVar>(std::move(domain), name);
    AddIntVar(v);
    return v;
}
}

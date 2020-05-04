#pragma once

#include <vector>
#include <map>
#include <string>

#include "icsp/clause.h"
#include "common/domain.h"

namespace csugar
{

class ICSP
{
public:
    ICSP() : auxiliary_id_(0) {}

    void AddBoolVar(std::shared_ptr<BoolVar> var);
    void AddIntVar(std::shared_ptr<IntVar> var);

    bool HasBoolVar(std::string name);
    std::shared_ptr<BoolVar> GetBoolVar(std::string name);
    bool HasIntVar(std::string name);
    std::shared_ptr<IntVar> GetIntVar(std::string name);

    std::shared_ptr<BoolVar> AuxiliaryBoolVar();
    std::shared_ptr<IntVar> AuxiliaryIntVar(std::shared_ptr<Domain> domain);

private:
    std::string AuxiliaryVarName() const;

    std::vector<Clause> clauses_;
    std::vector<std::shared_ptr<BoolVar>> bool_vars_;
    std::vector<std::shared_ptr<IntVar>> int_vars_;
    std::map<std::string, std::shared_ptr<BoolVar>> bool_var_map_;
    std::map<std::string, std::shared_ptr<IntVar>> int_var_map_;
    int auxiliary_id_;
};

}

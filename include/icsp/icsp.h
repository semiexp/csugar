#pragma once

#include <vector>
#include <map>
#include <string>

#include "csp/csp.h"
#include "icsp/clause.h"
#include "icsp/var.h"
#include "common/domain.h"

namespace csugar {

class ICSP {
public:
    ICSP() : auxiliary_id_(0), unsatisfiable_(false), encoded_clauses_(0), encoded_bool_vars_(0), encoded_int_vars_(0) {}

    void AddClause(const Clause& clause) { clauses_.push_back(clause); }
    void AddClause(Clause&& clause) { clauses_.push_back(clause); }

    void SetClauses(std::vector<Clause>&& clauses) { clauses_ = clauses; }
    int NumClauses() const { return clauses_.size(); }
    Clause& GetClause(int i) { return clauses_[i]; }
    const Clause& GetClause(int i) const { return clauses_[i]; }

    int NumEncodedClauses() const { return encoded_clauses_; }
    int NumEncodedBoolVars() const { return encoded_bool_vars_; }
    int NumEncodedIntVars() const { return encoded_int_vars_; }
    void SetAllEncoded() {
        encoded_clauses_ = clauses_.size();
        encoded_bool_vars_ = bool_vars_.size();
        encoded_int_vars_ = int_vars_.size();
    }

    int NumBoolVars() const { return bool_vars_.size(); }
    std::shared_ptr<ICSPBoolVar> GetBoolVar(int i) { return bool_vars_[i]; }
    std::shared_ptr<const ICSPBoolVar> GetBoolVar(int i) const { return bool_vars_[i]; }

    int NumIntVars() const { return int_vars_.size(); }
    std::shared_ptr<ICSPIntVar> GetIntVar(int i) { return int_vars_[i]; }
    std::shared_ptr<const ICSPIntVar> GetIntVar(int i) const { return int_vars_[i]; }

    std::shared_ptr<ICSPBoolVar> MakeBoolVar();
    std::shared_ptr<ICSPIntVar> MakeIntVar(std::unique_ptr<Domain>&& domain);

    void Propagate();
    bool IsUnsatisfiable() const { return unsatisfiable_; }

private:
    std::string AuxiliaryVarName() const;

    std::vector<Clause> clauses_;
    std::vector<std::shared_ptr<ICSPBoolVar>> bool_vars_;
    std::vector<std::shared_ptr<ICSPIntVar>> int_vars_;
    int auxiliary_id_;
    int encoded_clauses_, encoded_bool_vars_, encoded_int_vars_;
    bool unsatisfiable_;
};

}

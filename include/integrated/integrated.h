#pragma once

#include "conv/converter.h"
#include "conv/encoder.h"
#include "conv/simplifier.h"
#include "csp/csp.h"
#include "csp/var.h"
#include "icsp/icsp.h"
#include "sat/mapping.h"
#include "sat/sat.h"
#include "sat/solver.h"

#include <map>
#include <string>
#include <memory>
#include <optional>

namespace csugar {

class CSPAnswer;

class IntegratedCSPSolver {
public:
    IntegratedCSPSolver();

    void Parse(const std::string& in);

    CSPBoolVar GetBoolVar(const std::string& name) const { return bool_var_map_.at(name); }
    CSPIntVar GetIntVar(const std::string& name) const { return int_var_map_.at(name); }
    bool HasBoolVar(const std::string& name) const { return bool_var_map_.count(name) > 0; }
    bool HasIntVar(const std::string& name) const { return int_var_map_.count(name) > 0; }
    std::vector<std::string> BoolVars() const;
    std::vector<std::string> IntVars() const;
    CSPBoolVar MakeBoolVar(const std::string& name);
    CSPIntVar MakeIntVar(const std::string& name, std::unique_ptr<Domain>&& domain);

    void AddConstraint(std::shared_ptr<Expr> expr);
    void SetTargetVars(const std::vector<std::string>& vars) { target_vars_ = vars; }
    void ClearTargetVars() { target_vars_.reset(); }

    CSPAnswer Solve();

private:
    std::map<std::string, CSPBoolVar> bool_var_map_;
    std::map<std::string, CSPIntVar> int_var_map_;
    std::optional<std::vector<std::string>> target_vars_;

    std::unique_ptr<CSP> csp_;
    std::unique_ptr<ICSP> icsp_;
    std::unique_ptr<SAT> sat_;
    std::unique_ptr<Converter> conv_;
    std::unique_ptr<Simplifier> simplifier_;
    std::unique_ptr<Mapping> mapping_;
    std::unique_ptr<Encoder> encoder_;
    std::unique_ptr<Solver> solver_;
};

class CSPAnswer {
public:
    bool IsSat() const { return sat_; }
 
    friend class IntegratedCSPSolver;

    bool GetBool(const std::string& name) const { return bool_ans_.at(name); }
    int GetInt(const std::string& name) const { return int_ans_.at(name); }

private:
    CSPAnswer() : sat_(false) {}
    CSPAnswer(std::map<std::string, bool>&& bool_ans, std::map<std::string, int>&& int_ans) :
        sat_(true), bool_ans_(std::move(bool_ans)), int_ans_(std::move(int_ans)) {}

    bool sat_;
    std::map<std::string, bool> bool_ans_;
    std::map<std::string, int> int_ans_;
};

}

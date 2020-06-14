#pragma once

#include <vector>

#include "sat/satlit.h"
#include "sat/constraints.h"

namespace csugar {

class SAT {
public:
    SAT() : n_variables_(1), solved_variables_(0), solved_clauses_(0), solved_constraints_(0){
        clauses_.push_back({SATLit(0)});
    }
    static constexpr SATLit True() { return SATLit(0, false); }
    static constexpr SATLit False() { return SATLit(0, true); }

    int NumVariables() const { return n_variables_; }
    int NumClauses() const { return clauses_.size(); }
    int NumConstraints() const { return constraints_.size(); }
    const std::vector<SATLit>& GetClause(int i) const { return clauses_[i]; }
    const std::shared_ptr<NonClauseConstraint>& GetConstraint(int i) const { return constraints_[i]; }
    void AddVariables(int n = 1) { n_variables_ += n; }
    void AddClause(const std::vector<SATLit>& clause) { clauses_.push_back(clause); }
    void AddConstraint(std::shared_ptr<NonClauseConstraint> constraint) { constraints_.push_back(constraint); }

    int NumSolvedVariables() const { return solved_variables_; }
    int NumSolvedClauses() const { return solved_clauses_; }
    int NumSolverConstraints() const { return solved_constraints_; }
    void SetAllSolved() {
        solved_variables_ = n_variables_;
        solved_clauses_ = clauses_.size();
        solved_constraints_ = constraints_.size();
    }

private:
    std::vector<std::vector<SATLit>> clauses_;
    std::vector<std::shared_ptr<NonClauseConstraint>> constraints_;
    int solved_variables_, solved_clauses_, solved_constraints_;
    int n_variables_;
};

}

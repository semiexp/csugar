#pragma once

#include <vector>

#include "sat/satlit.h"

namespace csugar {

class SAT {
public:
    SAT() : n_variables_(1), solved_variables_(0), solved_clauses_(0) {
        clauses_.push_back({SATLit(0)});
    }
    static constexpr SATLit True() { return SATLit(0, false); }
    static constexpr SATLit False() { return SATLit(0, true); }

    int NumVariables() const { return n_variables_; }
    int NumClauses() const { return clauses_.size(); }
    const std::vector<SATLit>& GetClause(int i) const { return clauses_[i]; }
    int AddVariables(int n = 1) { n_variables_ += n; }
    void AddClause(const std::vector<SATLit>& clause) { clauses_.push_back(clause); }

    int NumSolvedVariables() const { return solved_variables_; }
    int NumSolvedClauses() const { return solved_clauses_; }
    void SetAllSolved() {
        solved_variables_ = n_variables_;
        solved_clauses_ = clauses_.size();
    }

private:
    std::vector<std::vector<SATLit>> clauses_;
    int solved_variables_, solved_clauses_;
    int n_variables_;
};

}

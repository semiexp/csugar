#include "sat/solver.h"

#include <vector>

#include "minisat/core/Solver.h"

#include "sat/satlit.h"

namespace csugar { 

std::vector<bool> Solver::Solve(bool incremental) {
    // TODO: incremental solver
    if (!actual_solver_ || !incremental) {
        actual_solver_ = std::move(std::make_unique<Minisat::Solver>());
    }
    for (int i = incremental ? sat_.NumSolvedVariables() : 0; i < sat_.NumVariables(); ++i) {
        actual_solver_->newVar();
    }
    for (int i = incremental ? sat_.NumSolvedClauses() : 0; i < sat_.NumClauses(); ++i) {
        const std::vector<SATLit>& clause = sat_.GetClause(i);
        bool is_true = false;
        for (SATLit lit : clause) {
            if (lit == SAT::True()) is_true = true;
        }
        if (is_true) continue;
        Minisat::vec<Minisat::Lit> c;
        for (SATLit lit : clause) {
            if (lit != SAT::False()) {
                c.push(Minisat::mkLit(lit.GetVariable(), lit.IsNegative()));
            }
        }
        actual_solver_->addClause_(c);
    }
    sat_.SetAllSolved();

    if (!actual_solver_->simplify()) {
        return std::vector<bool>();
    }

    bool is_sat = actual_solver_->solve();
    if (!is_sat) {
        return std::vector<bool>();
    }

    std::vector<bool> ret(sat_.NumVariables());
    Minisat::lbool ltrue((uint8_t)0);
    for (int i = 0; i < sat_.NumVariables(); ++i) {
        ret[i] = (actual_solver_->model[i] == ltrue);
    }

    return ret;
}

}

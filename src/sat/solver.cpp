#include "sat/solver.h"

#include <vector>

#include "sat/satlit.h"

namespace csugar { 

std::vector<bool> Solver::Solve(bool incremental) {
    // TODO: incremental solver
    if (!actual_solver_ || !incremental) {
        actual_solver_ = std::move(std::make_unique<BACKEND::Solver>());
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
        BACKEND::vec<BACKEND::Lit> c;
        for (SATLit lit : clause) {
            if (lit != SAT::False()) {
                //printf("%d ", (lit.IsNegative() ? -1 : 1) * (lit.GetVariable() + 1));
                c.push(BACKEND::mkLit(lit.GetVariable(), lit.IsNegative()));
            }
        }
        //printf("0\n");
        actual_solver_->addClause_(c);
    }
    for (int i = incremental ? sat_.NumSolverConstraints() : 0; i < sat_.NumConstraints(); ++i) {
        actual_solver_->addConstraint(sat_.GetConstraint(i)->Emit());
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
    BACKEND::lbool ltrue((uint8_t)0);
    for (int i = 0; i < sat_.NumVariables(); ++i) {
        ret[i] = (actual_solver_->model[i] == ltrue);
    }

    return ret;
}

}

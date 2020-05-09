#include "sat/solver.h"

#include <vector>

#include "minisat/core/Solver.h"

#include "sat/satlit.h"

namespace csugar { 

std::vector<bool> Solver::Solve() {
    // TODO: incremental solver
    Minisat::Solver solver;
    for (int i = 0; i < sat_.n_variables; ++i) {
        solver.newVar();
    }
    for (auto& clause : sat_.clauses) {
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
        solver.addClause_(c);
    }

    if (!solver.simplify()) {
        return std::vector<bool>();
    }

    bool is_sat = solver.solve();
    if (!is_sat) {
        return std::vector<bool>();
    }

    std::vector<bool> ret(sat_.n_variables);
    Minisat::lbool ltrue((uint8_t)0);
    for (int i = 0; i < sat_.n_variables; ++i) {
        ret[i] = (solver.model[i] == ltrue);
    }

    return ret;
}

}

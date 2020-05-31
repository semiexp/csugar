#pragma once

#include "sat/satlit.h"
#include "sat/graph_solver.h"
#include "minisat/core/Constraint.h"

#include <vector>
#include <algorithm>
#include <memory>

namespace csugar {

class NonClauseConstraint {
public:
    virtual ~NonClauseConstraint() {}

    virtual std::unique_ptr<Minisat::Constraint> Emit() const = 0;
};

class ActiveVerticesConnectedConstraint : public NonClauseConstraint {
public:
    ActiveVerticesConnectedConstraint(const std::vector<SATLit>& lits,
                                      const std::vector<std::pair<int, int>>& edges) : lits_(lits), edges_(edges) {}
    ~ActiveVerticesConnectedConstraint() {}

    std::unique_ptr<Minisat::Constraint> Emit() const override {
        std::vector<Minisat::Lit> minisat_lits;
        for (const SATLit& l : lits_) {
            minisat_lits.push_back(Minisat::mkLit(l.GetVariable(), l.IsNegative()));
        }
        return std::make_unique<Minisat::ActiveVerticesConnected>(minisat_lits, edges_);
    }
private:
    std::vector<SATLit> lits_;
    std::vector<std::pair<int, int>> edges_;
};

}

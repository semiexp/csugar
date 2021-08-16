#pragma once

#include "sat/satlit.h"
#include "sat/graph_solver.h"
#include "sat/backend_switch.h"

#include <vector>
#include <algorithm>
#include <memory>

namespace csugar {

class NonClauseConstraint {
public:
    virtual ~NonClauseConstraint() {}

    virtual std::unique_ptr<BACKEND::Constraint> Emit() const = 0;
};

class ActiveVerticesConnectedConstraint : public NonClauseConstraint {
public:
    ActiveVerticesConnectedConstraint(const std::vector<SATLit>& lits,
                                      const std::vector<std::pair<int, int>>& edges) : lits_(lits), edges_(edges) {}
    ~ActiveVerticesConnectedConstraint() {}

    std::unique_ptr<BACKEND::Constraint> Emit() const override {
        std::vector<BACKEND::Lit> minisat_lits;
        for (const SATLit& l : lits_) {
            minisat_lits.push_back(BACKEND::mkLit(l.GetVariable(), l.IsNegative()));
        }
        return std::make_unique<BACKEND::ActiveVerticesConnected>(minisat_lits, edges_);
    }
private:
    std::vector<SATLit> lits_;
    std::vector<std::pair<int, int>> edges_;
};

}

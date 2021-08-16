#pragma once

#include "sat/backend_switch.h"

#include <vector>
#include <algorithm>

namespace BACKEND {

class ActiveVerticesConnected : public Constraint {
public:
    ActiveVerticesConnected(const std::vector<Lit> &lits, const std::vector<std::pair<int, int>> &edges);
    virtual ~ActiveVerticesConnected() = default;

    bool initialize(Solver& solver, vec<Lit>& out_watchers) override;
    bool propagate(Solver& solver, Lit p) override;
    void calcReason(Solver& solver, Lit p, vec<Lit>& out_reason) override;
    void undo(Solver& solver, Lit p) override;

private:
    enum NodeState {
        kUndecided, kActive, kInactive
    };
    const int kUnvisited = -1;

    int buildTree(int v, int parent, int cluster_id);

    std::vector<Lit> lits_;
    std::vector<std::vector<int>> adj_;
    std::vector<NodeState> state_;
    std::vector<int> decision_order_;
    std::vector<int> rank_, lowlink_, subtree_active_count_, cluster_id_, parent_;
    int next_rank_;
    BACKEND::Lit conflict_cause_lit_;  // The conflict detected in propagate() is caused because `conflict_cause_pos_`-th variable was actually `conflict_cause_lit_`
    int conflict_cause_pos_;
    int n_active_vertices_;
};

}

#include "sat/graph_solver.h"

#include <set>
#include <algorithm>
#include <vector>

namespace {

class UnionFind {
public:
    UnionFind(int n) : parent_(n, -1), n_active_(n, 0), n_active_clusters_(0) {}

    int Root(int p) const {
        while (parent_[p] >= 0) p = parent_[p];
        return p;
    }
    int NumActiveClusters() const {
        return n_active_clusters_;
    }
    void Merge(int p, int q) {
        p = Root(p);
        q = Root(q);
        if (p == q) return;
        if (parent_[p] > parent_[q]) std::swap(p, q);

        UpdateParent(p, parent_[p] + parent_[q]);
        UpdateParent(q, p);

        int nac_updated = n_active_clusters_;
        nac_updated -= (n_active_[p] > 0 ? 1 : 0);
        nac_updated -= (n_active_[q] > 0 ? 1 : 0);
        nac_updated += (n_active_[p] + n_active_[q] > 0 ? 1 : 0);
        UpdateNActive(p, n_active_[p] + n_active_[q]);
        UpdateNActive(q, 0);
        UpdateNActiveClusters(nac_updated);
    }
    void AddActiveCount(int p, int d) {
        int nac_updated = n_active_clusters_;
        p = Root(p);
        nac_updated -= (n_active_[p] > 0 ? 1 : 0);
        UpdateNActive(p, n_active_[p] + d);
        nac_updated += (n_active_[p] > 0 ? 1 : 0);
        UpdateNActiveClusters(nac_updated);
    }
    void Commit() { redo_.clear(); }
    void Redo() {
        while (!redo_.empty()) {
            int loc = redo_.back().first, val = redo_.back().second;
            redo_.pop_back();

            if (loc == -1) n_active_clusters_ = val;
            else if (loc % 2 == 0) parent_[loc / 2] = val;
            else n_active_[loc / 2] = val;
        }
    }

private:
    void UpdateParent(int p, int v) {
        if (parent_[p] == v) return;
        redo_.push_back({p * 2, parent_[p]});
        parent_[p] = v;
    }
    void UpdateNActive(int p, int v) {
        if (n_active_[p] == v) return;
        redo_.push_back({p * 2 + 1, n_active_[p]});
        n_active_[p] = v;
    }
    void UpdateNActiveClusters(int v) {
        if (n_active_clusters_ == v) return;
        redo_.push_back({-1, n_active_clusters_});
        n_active_clusters_ = v;
    }

    std::vector<int> parent_, n_active_;
    std::vector<std::pair<int, int>> redo_;
    int n_active_clusters_;
};

}

namespace Minisat {

ActiveVerticesConnected::ActiveVerticesConnected(const std::vector<Lit>& lits, const std::vector<std::pair<int, int>>& edges)
    : lits_(lits), adj_(lits.size()), state_(lits.size(), kUndecided), conflict_cause_pos_(-2),
      rank_(lits.size()), lowlink_(lits.size()), subtree_active_count_(lits.size()), cluster_id_(lits.size()), parent_(lits.size()) {
    for (auto& e : edges) {
        adj_[e.first].push_back(e.second);
        adj_[e.second].push_back(e.first);
    }
}

void ActiveVerticesConnected::getWatchers(Solver& solver, vec<Lit>& out_watchers) {
    for (int i = 0; i < lits_.size(); ++i) {
        lbool val = solver.value(lits_[i]);
        if (val != l_Undef) decision_order_.push_back(i);
        if (val == l_True) state_[i] = kActive;
        else if (val == l_False) state_[i] = kInactive;
    }
    std::set<Lit> lits_unique;
    for (Lit l : lits_) {
        lits_unique.insert(l);
        lits_unique.insert(~l);
    }
    for (Lit l : lits_unique) {
        out_watchers.push(l);
    }
}

bool ActiveVerticesConnected::propagate(Solver& solver, Lit p) {
    int n = lits_.size();
    solver.registerUndo(var(p), this);
    for (int i = 0; i < n; ++i) {
        if (var(lits_[i]) == var(p)) {
            lbool val = solver.value(lits_[i]);
            NodeState s;
            if (val == l_True) s = kActive;
            else if (val == l_False) s = kInactive;
            else abort();
            state_[i] = s;
            decision_order_.push_back(i);
        }
    }

    std::fill(rank_.begin(), rank_.end(), kUnvisited);
    std::fill(lowlink_.begin(), lowlink_.end(), kUndecided);
    std::fill(subtree_active_count_.begin(), subtree_active_count_.end(), 0);
    std::fill(cluster_id_.begin(), cluster_id_.end(), kUndecided);
    std::fill(parent_.begin(), parent_.end(), -1);
    next_rank_ = 0;

    int nonempty_cluster = -1;

    for (int i = 0; i < n; ++i) {
        if (state_[i] != kInactive && rank_[i] == kUnvisited) {
            buildTree(i, -1, i);
            int sz = subtree_active_count_[i];
            if (sz >= 1) {
                if (nonempty_cluster != -1) {
                    conflict_cause_pos_ = -1;
                    return false; // already disconnected
                }
                nonempty_cluster = i;
            }
        }
    }

    if (nonempty_cluster != -1) {
        for (int v = 0; v < n; ++v) {
            if (state_[v] != kUndecided) continue;

            if (cluster_id_[v] != nonempty_cluster) {
                // nodes outside the nonempty cluster should be inactive
                if (!solver.enqueue(~lits_[v], this)) {
                    conflict_cause_pos_ = v;
                    conflict_cause_lit_ = lits_[v];
                    return false;
                }
            } else {
                // check if node `v` is an articulation point
                int parent_side_count = subtree_active_count_[nonempty_cluster] - subtree_active_count_[v];
                int n_nonempty_subgraph = 0;
                for (auto w : adj_[v]) {
                    if (rank_[v] < rank_[w] && parent_[w] == v) {
                        // `w` is a child of `v`
                        if (lowlink_[w] < rank_[v]) {
                            // `w` is not separated from `v`'s parent even after removal of `v`
                            parent_side_count += subtree_active_count_[w];
                        } else {
                            if (subtree_active_count_[w] > 0) ++n_nonempty_subgraph;
                        }
                    }
                }
                if (parent_side_count > 0) ++n_nonempty_subgraph;
                if (n_nonempty_subgraph >= 2) {
                    // `v` is an articulation point
                    if (!solver.enqueue(lits_[v], this)) {
                        conflict_cause_pos_ = v;
                        conflict_cause_lit_ = ~lits_[v];
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

int ActiveVerticesConnected::buildTree(int v, int parent, int cluster_id) {
    rank_[v] = next_rank_++;
    cluster_id_[v] = cluster_id;
    parent_[v] = parent;
    int lowlink = rank_[v];
    int subtree_active_count = (state_[v] == kActive ? 1 : 0);

    for (int w : adj_[v]) {
        if (w == parent || state_[w] == kInactive) continue;
        if (rank_[w] == -1) {
            // unvisited
            lowlink = std::min(lowlink, buildTree(w, v, cluster_id));
            subtree_active_count += subtree_active_count_[w];
        } else {
            lowlink = std::min(lowlink, rank_[w]);
        }
    }

    subtree_active_count_[v] = subtree_active_count;
    return lowlink_[v] = lowlink;
}

void ActiveVerticesConnected::calcReason(Solver& solver, Lit p, vec<Lit>& out_reason) {
    if (p == lit_Undef && conflict_cause_pos_ == -2) abort();
    if (p == lit_Undef && conflict_cause_pos_ != -1) {
        decision_order_.push_back(conflict_cause_pos_);
        if (conflict_cause_lit_ == lits_[conflict_cause_pos_]) {
            state_[conflict_cause_pos_] = kActive;
        } else {
            state_[conflict_cause_pos_] = kInactive;
        }
    }
    int n = lits_.size();
    UnionFind union_find(n);
    std::vector<bool> activated(n, false);

    for (int i = 0; i < n; ++i) {
        if (state_[i] == kActive) union_find.AddActiveCount(i, 1);
        if (state_[i] != kInactive && (p == lit_Undef || p != lits_[i])) activated[i] = true;
    }
    for (int v = 0; v < n; ++v) {
        for (int w : adj_[v]) {
            if (activated[v] && activated[w]) union_find.Merge(v, w);
        }
    }
    for (int i = 0; i < n; ++i) {
        if (lits_[i] == ~p) {
            union_find.AddActiveCount(i, 1);
        }
    }
    union_find.Commit();
    if (union_find.NumActiveClusters() <= 1) {
        abort();
    }
    for (int i = decision_order_.size() - 1; i >= 0; --i) {
        int v = decision_order_[i];
        if (p != lit_Undef && var(p) == var(lits_[v])) {
            abort();
        }

        if (state_[v] == kActive) union_find.AddActiveCount(v, -1);
        for (int w : adj_[v]) {
            if (activated[w]) union_find.Merge(v, w);
        }

        if (union_find.NumActiveClusters() >= 2) {
            union_find.Commit();
            activated[v] = true;
        } else {
            union_find.Redo();
            if (state_[v] == kActive) out_reason.push(lits_[v]);
            else if (state_[v] == kInactive) out_reason.push(~lits_[v]);
        }
    }
    if (p == lit_Undef && conflict_cause_pos_ != -1) {
        decision_order_.pop_back();
        state_[conflict_cause_pos_] = kUndecided;
    }
}

void ActiveVerticesConnected::undo(Solver& solver, Lit p) {
    for (int i = 0; i < lits_.size(); ++i) {
        if (var(lits_[i]) == var(p)) {
            state_[i] = kUndecided;
            decision_order_.pop_back();
        }
    }
}

}

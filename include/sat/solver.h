#pragma once

#include <vector>
#include <memory>

#include "sat/sat.h"
#include "minisat/core/Solver.h"

namespace csugar {

class Solver {
public:
    Solver(SAT &sat) : sat_(sat), actual_solver_(nullptr) {}

    std::vector<bool> Solve(bool incremental = false);

private:
    SAT &sat_;
    std::unique_ptr<Minisat::Solver> actual_solver_;
};

}

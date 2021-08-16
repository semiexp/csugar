#pragma once

#include <vector>
#include <memory>

#include "sat/sat.h"
#include "sat/backend_switch.h"

namespace csugar {

class Solver {
public:
    Solver(SAT &sat) : sat_(sat), actual_solver_(nullptr) {}

    std::vector<bool> Solve(bool incremental = false);

private:
    SAT &sat_;
#ifdef USE_GLUCOSE
    std::unique_ptr<Glucose::Solver> actual_solver_;
#else
    std::unique_ptr<Minisat::Solver> actual_solver_;
#endif

};

}

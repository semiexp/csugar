#pragma once

#include <vector>

#include "sat/sat.h"

namespace csugar {

class Solver {
public:
    Solver(SAT &sat) : sat_(sat) {}

    std::vector<bool> Solve();

private:
    SAT &sat_;
};

}

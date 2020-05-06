#pragma once

#include <vector>

#include "sat/satlit.h"

namespace csugar {

struct SAT {
    SAT() : n_variables(1) {
        clauses.push_back({SATLit(0)});
    }
    static constexpr SATLit True() { return SATLit(0, false); }
    static constexpr SATLit False() { return SATLit(0, true); }

    std::vector<std::vector<SATLit>> clauses;
    int n_variables;
};

}

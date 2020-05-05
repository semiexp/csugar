#pragma once

#include <vector>

#include "icsp/icsp.h"
#include "icsp/clause.h"

namespace csugar {

class Simplifier {
public:
    Simplifier(ICSP& icsp) : icsp_(icsp) {}

    void Simplify();

private:
    std::vector<Clause> Simplify(Clause &clause);

    ICSP &icsp_;
};

}

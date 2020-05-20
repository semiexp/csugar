#include "icsp/icsp.h"

#include <string>
#include <algorithm>

#include "csp/csp.h"
#include "common/domain.h"

namespace csugar {

std::shared_ptr<ICSPBoolVar> ICSP::MakeBoolVar() {
    auto v = std::make_shared<ICSPBoolVar>(NumBoolVars());
    bool_vars_.push_back(v);
    return v;
}
std::shared_ptr<ICSPIntVar> ICSP::MakeIntVar(std::unique_ptr<Domain>&& domain) {
    auto v = std::make_shared<ICSPIntVar>(std::move(domain), NumIntVars());
    int_vars_.push_back(v);
    return v;
}
void ICSP::Propagate() {
    bool updated;
    do {
        updated = false;

        for (auto& clause : clauses_) {
            DomainBoundingResult prop = clause.Propagate();
            if (prop == kEmptyDomain) {
                unsatisfiable_ = true;
                return;
            } else if (prop == kUpdate) {
                updated = true;
            }
            if (clause.RemoveFalsefood()) updated = true;
        }

        if (!updated) break;
    } while (updated);

    clauses_.erase(
        std::remove_if(clauses_.begin(), clauses_.end(), [](Clause& clause) { return clause.IsValid(); }),
        clauses_.end()
    );
}
}

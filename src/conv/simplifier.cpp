#include "conv/simplifier.h"

#include <vector>

#include "icsp/bool_literal.h"
#include "icsp/clause.h"

namespace csugar {

void Simplifier::Simplify() {
    std::vector<Clause> new_clauses;
    for (int i = 0; i < icsp_.NumClauses(); ++i) {
        auto new_clauses_part = Simplify(icsp_.GetClause(i));
        new_clauses.insert(new_clauses.end(), new_clauses_part.begin(), new_clauses_part.end());
    }
    icsp_.SetClauses(std::move(new_clauses));
}
std::vector<Clause> Simplifier::Simplify(Clause &clause) {
    std::vector<Clause> new_clauses;
    if (clause.IsSimple()) {
        new_clauses.push_back(clause);
    } else {
        Clause c;
        for (int i = 0; i < clause.size(); ++i) {
            auto lit = clause[i];
            if (lit->IsSimple()) {
                c.Add(lit);
            } else {
                // TODO: simplify cache
                auto p = icsp_.AuxiliaryBoolVar();
                std::shared_ptr<Literal> pos = std::make_shared<BoolLiteral>(p, false);
                std::shared_ptr<Literal> neg = std::make_shared<BoolLiteral>(p, true);

                // TODO: equiv translation
                Clause c2;
                c2.Add(neg);
                c2.Add(lit);
                new_clauses.push_back(c2);

                c.Add(pos);
            }
        }
        new_clauses.push_back(c);
    }
    return new_clauses;
}

}

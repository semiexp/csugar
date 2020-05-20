#include "conv/encoder.h"

#include <algorithm>
#include <cassert>

#include "icsp/clause.h"
#include "icsp/bool_literal.h"
#include "icsp/linear_literal.h"
#include "icsp/linear_sum.h"
#include "common/util.h"

namespace csugar {

void Encoder::Encode(bool incremental) {
    for (int i = incremental ? icsp_.NumEncodedBoolVars() : 0; i < icsp_.NumBoolVars(); ++i) {
        EncodeBoolVar(icsp_.GetBoolVar(i));
    }
    for (int i = incremental ? icsp_.NumEncodedIntVars() : 0; i < icsp_.NumIntVars(); ++i) {
        EncodeIntVar(icsp_.GetIntVar(i));
    }
    for (int i = incremental ? icsp_.NumEncodedClauses() : 0; i < icsp_.NumClauses(); ++i) {
        EncodeClause(icsp_.GetClause(i));
    }
    icsp_.SetAllEncoded();
}
void Encoder::EncodeBoolVar(std::shared_ptr<ICSPBoolVar> var) {
    mapping_.RegisterMappingBool(var);
}
void Encoder::EncodeIntVar(std::shared_ptr<ICSPIntVar> var) {
    mapping_.RegisterMappingInt(var);
    var->SetEncoded();

    std::vector<int> domain = var->domain()->Enumerate();
    for (int i = 1; i < domain.size(); ++i) {
        AddSATClause({
            !GetCodeLE(var, domain[i - 1]),
            GetCodeLE(var, domain[i])
        });
    }
}
void Encoder::EncodeClause(const Clause& clause) {
    // check validity
    if (!clause.IsSimple()) {
        // TODO: error
    }

    std::vector<SATLit> sat_clause;
    int complex_index = -1;

    for (int i = 0; i < clause.size(); ++i) {
        auto lit = clause[i];

        if (lit->IsSimple()) {
            SATLit l = GetCode(lit);
            sat_clause.push_back(l);
        } else {
            complex_index = i;
        }
    }

    if (complex_index == -1) {
        AddSATClause(sat_clause);
    } else {
        EncodeLiteral(clause[complex_index], sat_clause);
    }
}
SATLit Encoder::GetCode(std::shared_ptr<const Literal> literal) {
    if (std::shared_ptr<const BoolLiteral> bool_literal = std::dynamic_pointer_cast<const BoolLiteral>(literal)) {
        SATLit code = mapping_.GetCode(bool_literal->var());
        if (bool_literal->negative()) code = !code;
        return code;
    } else if (std::shared_ptr<const LinearLiteral> linear_literal =
                    std::dynamic_pointer_cast<const LinearLiteral>(literal)) {
        if (linear_literal->op() == kLitEq || linear_literal->op() == kLitNe) {
            bool satisfied = (linear_literal->sum().GetB() == 0) == (linear_literal->op() == kLitEq);
            return satisfied ? sat_.True() : sat_.False();
        } else if (linear_literal->op() == kLitGe) {
            const LinearSum& sum = linear_literal->sum();
            if (sum.size() == 0) {
                return sum.GetB() >= 0 ? sat_.True() : sat_.False();
            } else {
                auto [v, a] = *sum.GetCoef().begin();
                return GetCodeLE(v, -a, sum.GetB());
            }
        } else if (linear_literal->op() == kLitLe) {
            const LinearSum& sum = linear_literal->sum();
            if (sum.size() == 0) {
                return sum.GetB() <= 0 ? sat_.True() : sat_.False();
            } else {
                auto [v, a] = *sum.GetCoef().begin();
                return GetCodeLE(v, a, -sum.GetB());
            }
        }
    } else {
        // error
    }
}
void Encoder::EncodeLiteral(std::shared_ptr<Literal> literal, const std::vector<SATLit>& clause) {
    if (std::shared_ptr<BoolLiteral> bool_literal = std::dynamic_pointer_cast<BoolLiteral>(literal)) {
        SATLit code = mapping_.GetCode(bool_literal->var());
        if (bool_literal->negative()) code = !code;
        std::vector<SATLit> clause2 = clause;
        clause2.push_back(code);
        AddSATClause(clause2);
    } else if (std::shared_ptr<LinearLiteral> linear_literal = std::dynamic_pointer_cast<LinearLiteral>(literal)) {
        switch (linear_literal->op()) {
        case kLitLe:
            EncodeLinearLeLiteral(linear_literal, clause);
            break;
        case kLitGe:
            EncodeLinearGeLiteral(linear_literal, clause);
            break;
        case kLitEq:
            EncodeLinearEqLiteral(linear_literal, clause);
            break;
        case kLitNe:
            EncodeLinearNeLiteral(linear_literal, clause);
            break;
        }
    }
}
void Encoder::EncodeLinearLeLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause) {
    if (literal->IsValid()) return;
    if (literal->IsSimple()) {
        std::vector<SATLit> clause2 = clause;
        clause2.push_back(GetCode(literal));
        AddSATClause(clause2);
    } else {
        LinearSum sum = literal->sum();
        int n = sum.size();
        std::vector<int> as;
        std::vector<std::shared_ptr<ICSPIntVar>> vars;

        for (auto v : sum.GetVariablesSorted()) {
            vars.push_back(v);
            as.push_back(sum.GetCoef()[v]);
        }

        std::vector<SATLit> clause2(n, sat_.False());
        clause2.insert(clause2.end(), clause.begin(), clause.end());
        EncodeLinearLe(as, vars, 0, sum.GetB(), clause2);
    }
}
void Encoder::EncodeLinearNeLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause) {
    if (literal->IsValid()) return;
    LinearSum sum = literal->sum();
    int n = sum.size();
    std::vector<int> as;
    std::vector<std::shared_ptr<ICSPIntVar>> vars;

    for (auto v : sum.GetVariablesSorted()) {
        vars.push_back(v);
        as.push_back(sum.GetCoef()[v]);
    }

    std::vector<SATLit> clause2(n * 2, sat_.False());
    clause2.insert(clause2.end(), clause.begin(), clause.end());
    EncodeLinearNe(as, vars, 0, sum.GetB(), clause2);
}
void Encoder::EncodeLinearGeLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause) {
    LinearSum e(0);
    e -= literal->sum();
    auto literal2 = std::make_shared<LinearLiteral>(e, kLitLe);
    EncodeLinearLeLiteral(literal2, clause);
}
void Encoder::EncodeLinearEqLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause) {
    EncodeLiteral(std::make_shared<LinearLiteral>(literal->sum(), kLitLe), clause);
    EncodeLiteral(std::make_shared<LinearLiteral>(literal->sum(), kLitGe), clause);
}
void Encoder::EncodeLinearLe(const std::vector<int>& as,
                             std::vector<std::shared_ptr<ICSPIntVar>>& vars,
                             int idx,
                             int b,
                             std::vector<SATLit>& clause) {
    if (idx >= (int)vars.size() - 1) {
        clause[idx] = GetCodeLE(vars[idx], as[idx], -b);
        if (clause[idx] != sat_.True()) {
            AddSATClause(clause);
        }
    } else {
        int lb0 = b, ub0 = b;
        for (int j = idx + 1; j < vars.size(); ++j) {
            int a = as[j];
            if (a > 0) {
                lb0 += a * vars[j]->domain()->GetLowerBound();
                ub0 += a * vars[j]->domain()->GetUpperBound();
            } else {
                lb0 += a * vars[j]->domain()->GetUpperBound();
                ub0 += a * vars[j]->domain()->GetLowerBound();
            }
        }
        int a = as[idx];
        auto& domain = vars[idx]->domain();
        int lb = domain->GetLowerBound(), ub = domain->GetUpperBound();

        assert(a != 0);
        if (a > 0) {
            ub = std::min(ub, FloorDiv(-lb0, a));
            // TODO: efficient impl
            for (int c : domain->Enumerate()) {
                if (!(lb <= c && c <= ub)) continue;
                clause[idx] = GetCodeLE(vars[idx], c - 1);
                if (clause[idx] != sat_.True()) {
                    EncodeLinearLe(as, vars, idx + 1, b + a * c, clause);
                }
            }
            clause[idx] = GetCodeLE(vars[idx], ub);
            if (clause[idx] != sat_.True()) {
                EncodeLinearLe(as, vars, idx + 1, b + a * (ub + 1), clause);
            }
        } else {
            lb = std::max(lb, CeilDiv(-lb0, a));
            clause[idx] = !GetCodeLE(vars[idx], lb - 1);
            if (clause[idx] != sat_.True()) {
                EncodeLinearLe(as, vars, idx + 1, b + a * (lb - 1), clause);
            }
            for (int c : domain->Enumerate()) {
                if (!(lb <= c && c <= ub)) continue;
                clause[idx] = !GetCodeLE(vars[idx], c);
                if (clause[idx] != sat_.True()) {
                    EncodeLinearLe(as, vars, idx + 1, b + a * c, clause);
                }
            }
        }
    }
}
void Encoder::EncodeLinearNe(const std::vector<int>& as,
                             std::vector<std::shared_ptr<ICSPIntVar>>& vars,
                             int idx,
                             int b,
                             std::vector<SATLit>& clause) {
    // aa
    int idx2 = idx * 2;
    if (idx >= (int)vars.size() - 1) {
        clause[idx2] = GetCodeLE(vars[idx], as[idx], -b-1);
        clause[idx2 + 1] = GetCodeLE(vars[idx], -as[idx], b-1);
        if (clause[idx2] != sat_.True() && clause[idx2 + 1] != sat_.True()) {
            AddSATClause(clause);
        }
    } else {
        int a = as[idx];
        auto& domain = vars[idx]->domain();
        for (int c : domain->Enumerate()) {
            clause[idx2] = GetCodeLE(vars[idx], 1, c - 1);
            clause[idx2 + 1] = GetCodeLE(vars[idx], -1, -c - 1);
            if (clause[idx2] != sat_.True() && clause[idx2 + 1] != sat_.True()) {
                EncodeLinearNe(as, vars, idx + 1, b + a * c, clause);
            }
        }
    }
}
}

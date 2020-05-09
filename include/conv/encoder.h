#pragma once

#include <memory>
#include <cassert>

#include "icsp/icsp.h"
#include "icsp/clause.h"
#include "icsp/literal.h"
#include "icsp/linear_literal.h"
#include "sat/sat.h"
#include "sat/satlit.h"
#include "sat/mapping.h"
#include "common/var.h"
#include "common/util.h"

namespace csugar {

class Encoder {
public:
    Encoder(ICSP& icsp, SAT& sat, Mapping& mapping) : icsp_(icsp), sat_(sat), mapping_(mapping) {}

    void Encode(bool incremental = false);
    void EncodeBoolVar(std::shared_ptr<const BoolVar> var);
    void EncodeIntVar(std::shared_ptr<const IntVar> var);
    void EncodeClause(const Clause& clause);
private:
    SATLit GetCodeLE(std::shared_ptr<const IntVar> var, int c) {
        return mapping_.GetCodeLE(var, c);
    }
    SATLit GetCodeLE(std::shared_ptr<const IntVar> var, int a, int b) { // v * a <= b
        if (a == 0) {
            if (b >= 0) return sat_.True();
            else return sat_.False();
        } else if (a > 0) {
            return GetCodeLE(var, FloorDiv(b, a));
        } else {
            return !GetCodeLE(var, CeilDiv(b, a) - 1);
        }
    }
    SATLit GetCode(std::shared_ptr<const Literal> literal);
    void AddSATClause(const std::vector<SATLit>& clause) { sat_.AddClause(clause); }
    void EncodeLiteral(std::shared_ptr<Literal> literal, const std::vector<SATLit>& clause);
    void EncodeLinearLeLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause);
    void EncodeLinearNeLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause);
    void EncodeLinearGeLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause);
    void EncodeLinearEqLiteral(std::shared_ptr<LinearLiteral> literal, const std::vector<SATLit>& clause);

    // as[idx] * vars[idx] + ... + b <= 0
    void EncodeLinearLe(const std::vector<int>& as,
                        std::vector<std::shared_ptr<IntVar>>& vars,
                        int idx,
                        int b,
                        std::vector<SATLit>& clause);
    void EncodeLinearNe(const std::vector<int>& as,
                        std::vector<std::shared_ptr<IntVar>>& vars,
                        int idx,
                        int b,
                        std::vector<SATLit>& clause);
    ICSP &icsp_;
    SAT &sat_;
    Mapping &mapping_;
};

}

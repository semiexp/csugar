#include "tests.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "csp/expr.h"
#include "csp/var.h"
#include "common/interval_domain.h"
#include "icsp/icsp.h"
#include "conv/converter.h"

using namespace csugar;

void ConvertTest1();
void ConvertTest2();

void RunConvertTests() {
    ConvertTest1();
    ConvertTest2();
}

void ConvertTest1() {
    CSP csp;
    auto a = csp.MakeBoolVar();
    auto b = csp.MakeBoolVar();

    // a xor b
    csp.AddExpr(Expr::Make(kXor, {Expr::VarBool(a), Expr::VarBool(b)}));
    ICSP icsp;
    Converter conv(csp, icsp);
    conv.Convert();

    assert(icsp.NumClauses() == 2);
    assert(icsp.GetClause(0).str() == "[b0 b1]");
    assert(icsp.GetClause(1).str() == "[!b0 !b1]");
}

void ConvertTest2() {
    // a + a < b + 2
    CSP csp;
    auto a = csp.MakeIntVar(std::make_unique<IntervalDomain>(0, 10));
    auto b = csp.MakeIntVar(std::make_unique<IntervalDomain>(0, 10));

    csp.AddExpr(Expr::Make(kLt, {
        Expr::Make(kAdd, {Expr::VarInt(a), Expr::VarInt(a)}),
        Expr::Make(kAdd, {Expr::VarInt(b), Expr::ConstInt(2)}),
    }));

    ICSP icsp;
    Converter conv(csp, icsp);
    conv.Convert();

    assert(icsp.NumClauses() == 1);
    assert(icsp.GetClause(0).str() == "[i0*2+i1*-1+-1<=0]");
}

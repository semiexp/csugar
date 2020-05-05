#include "tests.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "csp/expr.h"
#include "common/var.h"
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
    // a xor b
    auto ex = Expr::Make(kXor, {Expr::VarBool("a"), Expr::VarBool("b")});

    ICSP icsp;
    icsp.AddBoolVar(std::make_shared<BoolVar>("a"));
    icsp.AddBoolVar(std::make_shared<BoolVar>("b"));

    Converter conv(icsp);
    conv.Convert(ex);

    assert(icsp.NumClauses() == 2);
    assert(icsp.GetClause(0).str() == "[a b]");
    assert(icsp.GetClause(1).str() == "[!a !b]");
}

void ConvertTest2() {
    // a + a < b + 2
    auto ex = Expr::Make(kLt, {
        Expr::Make(kAdd, {Expr::VarInt("a"), Expr::VarInt("a")}),
        Expr::Make(kAdd, {Expr::VarInt("b"), Expr::ConstInt(2)}),
    });

    ICSP icsp;
    icsp.AddIntVar(std::make_shared<IntVar>(std::make_unique<IntervalDomain>(0, 10), "a"));
    icsp.AddIntVar(std::make_shared<IntVar>(std::make_unique<IntervalDomain>(0, 10), "b"));

    Converter conv(icsp);
    conv.Convert(ex);

    assert(icsp.NumClauses() == 1);
    assert(icsp.GetClause(0).str() == "[a*2+b*-1+-1<=0]");
}

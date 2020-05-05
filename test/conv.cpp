#include "tests.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "csp/expr.h"
#include "common/var.h"
#include "icsp/icsp.h"
#include "conv/converter.h"

using namespace csugar;

void ConvertTest1();

void RunConvertTests() {
    ConvertTest1();
}

void ConvertTest1() {
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

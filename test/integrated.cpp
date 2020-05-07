#include "tests.h"

#include <cassert>
#include <memory>
#include <map>
#include <vector>

#include "csp/csp.h"
#include "csp/expr.h"
#include "csp/parser.h"
#include "icsp/icsp.h"
#include "common/var.h"
#include "common/interval_domain.h"
#include "conv/converter.h"
#include "conv/simplifier.h"
#include "conv/encoder.h"
#include "sat/sat.h"
#include "sat/mapping.h"
#include "sat/solver.h"

using namespace csugar;

namespace {
void RunIntegratedSolvingTest1();
}
void RunIntegratedSolvingTests() {
    RunIntegratedSolvingTest1();
}

namespace {
bool EvaluateBoolExpr(std::shared_ptr<Expr> expr, const std::map<std::string, bool>& bool_map, const std::map<std::string, int>& int_map);
int EvaluateIntExpr(std::shared_ptr<Expr> expr, const std::map<std::string, bool>& bool_map, const std::map<std::string, int>& int_map);

bool EvaluateBoolExpr(std::shared_ptr<Expr> expr, const std::map<std::string, bool>& bool_map, const std::map<std::string, int>& int_map) {
    if (expr->type() == kConstantBool) {
        return expr->AsConstantBool();
    } else if (expr->type() == kVariableBool) {
        return bool_map.at(expr->VariableName());
    } else if (expr->type() == kNot) {
        return !EvaluateBoolExpr((*expr)[0], bool_map, int_map);
    } else if (expr->type() == kAnd || expr->type() == kOr || expr->type() == kImp || expr->type() == kXor || expr->type() == kIff) {
        std::vector<bool> subs;
        for (int i = 0; i < expr->size(); ++i) {
            subs.push_back(EvaluateBoolExpr((*expr)[i], bool_map, int_map));
        }
        bool ret;
        if (expr->type() == kAnd) {
            ret = true;
            for (auto x : subs) ret = ret && x;
        } else if (expr->type() == kOr) {
            ret = false;
            for (auto x : subs) ret = ret || x;
        } else if (expr->type() == kImp) {
            assert(subs.size() == 2);
            ret = (!subs[0]) || subs[1];
        } else if (expr->type() == kXor) {
            assert(subs.size() == 2);
            ret = (subs[0] != subs[1]);
        } else if (expr->type() == kIff) {
            assert(subs.size() == 2);
            ret = (subs[0] == subs[1]);
        }
        return ret;
    } else if (expr->type() == kEq || expr->type() == kNe || expr->type() == kLe || expr->type() == kLt || expr->type() == kGe || expr->type() == kGt) {
        assert(expr->size() == 2);
        int lhs = EvaluateIntExpr((*expr)[0], bool_map, int_map);
        int rhs = EvaluateIntExpr((*expr)[1], bool_map, int_map);
        switch (expr->type()) {
        case kEq: return lhs == rhs;
        case kNe: return lhs != rhs;
        case kLe: return lhs <= rhs;
        case kLt: return lhs < rhs;
        case kGe: return lhs >= rhs;
        case kGt: return lhs > rhs;
        }
    }
    abort();
}
int EvaluateIntExpr(std::shared_ptr<Expr> expr, const std::map<std::string, bool>& bool_map, const std::map<std::string, int>& int_map) {
    if (expr->type() == kConstantInt) {
        return expr->AsConstantInt();
    } else if (expr->type() == kVariableInt) {
        return int_map.at(expr->VariableName());
    } else if (expr->type() == kAdd || expr->type() == kSub) {
        if (expr->size() == 0) return 0;
        std::vector<int> subs;
        for (int i = 0; i < expr->size(); ++i) {
            subs.push_back(EvaluateIntExpr((*expr)[i], bool_map, int_map));
        }
        int ret;
        if (expr->type() == kAdd) {
            ret = subs[0];
            for (int i = 1; i < subs.size(); ++i) ret += subs[i];
        } else if (expr->type() == kSub) {
            ret = subs[0];
            for (int i = 1; i < subs.size(); ++i) ret -= subs[i];
        }
        return ret;
    } else if (expr->type() == kIf) {
        assert(expr->size() == 3);
        bool c = EvaluateBoolExpr((*expr)[0], bool_map, int_map);
        return EvaluateIntExpr((*expr)[c ? 1 : 2], bool_map, int_map);
    }
    abort();
}

void TestCSPInstance(CSP& csp, bool expected_sat) {
    ICSP icsp;
    icsp.LoadVars(csp);

    Converter conv(icsp);
    for (auto expr : csp.Exprs()) {
        conv.Convert(expr);
    }

    icsp.Propagate();

    Simplifier simp(icsp);
    simp.Simplify();

    if (expected_sat) assert(!icsp.IsUnsatisfiable());
    else if (icsp.IsUnsatisfiable()) return;

    SAT sat;
    Mapping mapping(sat);
    Encoder enc(icsp, sat, mapping);
    enc.Encode();

    Solver solver(sat);
    auto sol = solver.Solve();

    if (!expected_sat) {
        assert(sol.size() == 0);
    } else {
        assert(sol.size() > 0);

        std::map<std::string, bool> bool_map;
        std::map<std::string, int> int_map;
        for (auto p : csp.BoolVars()) {
            bool_map[p.first] = mapping.Retrieve(p.second, sol);
        }
        for (auto p : csp.IntVars()) {
            int_map[p.first] = mapping.Retrieve(p.second, sol);
        }
        for (auto expr : csp.Exprs()) {
            assert(EvaluateBoolExpr(expr, bool_map, int_map));
        }
    }
}

void RunIntegratedSolvingTest1() {
    {
        CSP csp;
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 12), "a");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 12), "b");
        csp.AddExpr(StringToExpr("(< (+ a a) b)", csp));
        csp.AddExpr(StringToExpr("(< (+ b 1) (+ a a a))", csp));
        csp.AddExpr(StringToExpr("(< (- b a) 5)", csp));

        TestCSPInstance(csp, true);
    }
    {
        CSP csp;
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "a");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "b");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "c");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "d");
        csp.AddExpr(StringToExpr("(and (< a b) (< b c) (< c d))", csp));

        TestCSPInstance(csp, true);
    }
    {
        CSP csp;
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "a");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "b");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "c");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "d");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 3), "e");
        csp.AddExpr(StringToExpr("(and (< a b) (< b c) (< c d) (< d e))", csp));

        TestCSPInstance(csp, false);
    }
    {
        CSP csp;
        csp.AddBoolVar("x");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 10), "a");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 10), "b");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 10), "c");
        csp.AddExpr(StringToExpr("(== a (+ b b (if x 1 0)))", csp));
        csp.AddExpr(StringToExpr("(== a (+ c c (if x 1 0)))", csp));

        TestCSPInstance(csp, true);
    }
    {
        CSP csp;
        csp.AddBoolVar("x");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 10), "a");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 10), "b");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 10), "c");
        csp.AddExpr(StringToExpr("(== a (+ b b (if x 1 0)))", csp));
        csp.AddExpr(StringToExpr("(== a (+ c c (if x 0 1)))", csp));

        TestCSPInstance(csp, false);
    }
    {
        CSP csp;
        csp.AddBoolVar("x");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 20), "a");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 20), "b");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 20), "c");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 20), "d");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 20), "e");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 20), "f");
        csp.AddIntVar(std::make_unique<IntervalDomain>(0, 20), "g");
        csp.AddExpr(StringToExpr("(>= (+ a b c d e f g) (if x 135 0))", csp));
        csp.AddExpr(StringToExpr("(>= (+ a b c d e f g) (if x 0 135))", csp));

        TestCSPInstance(csp, true);
    }
}

}

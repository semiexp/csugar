#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>

#include "csp/csp.h"
#include "csp/parser.h"
#include "common/interval_domain.h"
#include "icsp/icsp.h"
#include "conv/converter.h"
#include "conv/simplifier.h"
#include "conv/encoder.h"
#include "sat/sat.h"
#include "sat/mapping.h"
#include "sat/solver.h"

using namespace csugar;

std::vector<std::string> SplitDepthOneExpr(const std::string& s) {
    std::vector<std::string> ret;
    std::string token;

    for (int i = 1; i < s.size() - 1; ++i) {
        if (s[i] == ' ') {
            ret.push_back(token);
            token.clear();
        } else {
            token.push_back(s[i]);
        }
    }
    if (token.size() > 0) ret.push_back(token);
    return ret;
}

bool IsComment(const std::string& s) {
    return s.size() >= 1 && s[0] == ';';
}
bool IsBoolDefinition(const std::string& s) {
    return s.substr(0, 5) == "(bool";
}
bool IsIntDefinition(const std::string& s) {
    return s.substr(0, 4) == "(int";
}

CSP InputCSP() {
    CSP ret;

    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.size() == 0 || IsComment(line)) {
            continue;
        } else if (IsBoolDefinition(line)) {
            auto toks = SplitDepthOneExpr(line);
            if (toks.size() != 2) {
                std::cerr << "invalid bool token" << std::endl;
                exit(1);
            }
            ret.AddBoolVar(toks[1]);
        } else if (IsIntDefinition(line)) {
            auto toks = SplitDepthOneExpr(line);
            if (toks.size() != 4) {
                std::cerr << "invalid bool token" << std::endl;
                exit(1);
            }
            ret.AddIntVar(std::make_unique<IntervalDomain>(std::stoi(toks[2]), std::stoi(toks[3])), toks[1]);
        } else {
            ret.AddExpr(StringToExpr(line, ret));
        }
    }

    return std::move(ret);
}

int main() {
    CSP input = InputCSP();

    ICSP icsp;
    icsp.LoadVars(input);

    Converter conv(icsp);
    for (auto expr : input.Exprs()) {
        conv.Convert(expr);
    }

    icsp.Propagate();

    Simplifier simp(icsp);
    simp.Simplify();

    if (icsp.IsUnsatisfiable()) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return 0;
    }

    SAT sat;
    Mapping mapping(sat);
    Encoder enc(icsp, sat, mapping);
    enc.Encode();

    Solver solver(sat);
    auto sol = solver.Solve();
    if (sol.size() == 0) {
        std::cout << "s UNSATISFIABLE" << std::endl;
    } else {
        std::cout << "s SATISFIABLE" << std::endl;
        for (auto& vars : input.BoolVars()) {
            std::cout << "a " << vars.first << '\t' << (mapping.Retrieve(icsp.GetBoolVar(vars.first), sol) ? "true" : "false") << std::endl; 
        }
        for (auto& vars : input.IntVars()) {
            std::cout << "a " << vars.first << '\t' << mapping.Retrieve(icsp.GetIntVar(vars.first), sol) << std::endl; 
        }
        std::cout << "a" << std::endl;
    }
    return 0;
}

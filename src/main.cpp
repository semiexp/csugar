#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <map>

#include "csp/csp.h"
#include "csp/parser.h"
#include "csp/expr.h"
#include "common/interval_domain.h"
#include "common/enumerative_domain.h"
#include "icsp/icsp.h"
#include "conv/converter.h"
#include "conv/simplifier.h"
#include "conv/encoder.h"
#include "sat/sat.h"
#include "sat/mapping.h"
#include "sat/solver.h"

using namespace csugar;

std::vector<std::string> Tokenize(const std::string& s) {
    std::vector<std::string> ret;
    std::string token;
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == ' ') {
            if (token.size() > 0) ret.push_back(token);
            token.clear();
        } else {
            token.push_back(s[i]);
        }
    }
    if (token.size() > 0) ret.push_back(token);
    return ret;
}
std::vector<std::string> SplitDepthOneExpr(const std::string& s) {
    return Tokenize(s.substr(1, (int)s.size() - 2));
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

CSP InputCSP(bool& has_answer_key, std::vector<std::string>& answer_keys) {
    has_answer_key = false;
    answer_keys.clear();

    CSP ret;

    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.size() == 0 || IsComment(line)) {
            continue;
        } else if (line[0] == '#') {
            auto keys = Tokenize(line.substr(1));
            answer_keys.insert(answer_keys.end(), keys.begin(), keys.end());
            has_answer_key = true;
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
            ret.AddIntVar(std::make_unique<EnumerativeDomain>(std::stoi(toks[2]), std::stoi(toks[3])), toks[1]);
        } else {
            ret.AddExpr(StringToExpr(line, ret));
        }
    }

    return std::move(ret);
}

void SolveIrrefutably(CSP& csp, std::vector<std::string>& answer_keys) {
    ICSP icsp;
    icsp.LoadVars(csp);

    Converter conv(icsp);
    conv.Convert(csp);

    icsp.Propagate();

    Simplifier simp(icsp);
    simp.Simplify();

    if (icsp.IsUnsatisfiable()) {
        std::cout << "unsat" << std::endl;
        return;
    }

    SAT sat;
    Mapping mapping(sat);
    Encoder enc(icsp, sat, mapping);
    enc.Encode();

    Solver solver(sat);
    std::vector<bool> sol = solver.Solve();

    if (sol.size() == 0)  {
        std::cout << "unsat" << std::endl;
        return;
    }

    std::map<std::string, bool> not_refuted_bool;
    std::map<std::string, int> not_refuted_int;
    for (auto& name : answer_keys) {
        if (icsp.HasBoolVar(name)) {
            not_refuted_bool.insert({name, mapping.Retrieve(icsp.GetBoolVar(name), sol)});
        } else if (icsp.HasIntVar(name)) {
            not_refuted_int.insert({name, mapping.Retrieve(icsp.GetIntVar(name), sol)});
        } else {
            std::cout << "variable " << name << " not found" << std::endl;
            return;
        }
    }
    while (true) {
        std::vector<std::shared_ptr<Expr>> refuting_exprs;
        for (auto& p : not_refuted_bool) {
            refuting_exprs.push_back(Expr::Make(kXor, {Expr::VarBool(p.first), Expr::ConstBool(p.second)}));
        }
        for (auto& p : not_refuted_int) {
            refuting_exprs.push_back(Expr::Make(kNe, {Expr::VarInt(p.first), Expr::ConstInt(p.second)}));
        }
        csp.AddExpr(std::make_shared<Expr>(kOr, refuting_exprs));
        conv.Convert(csp, true);
        simp.Simplify(true);
        enc.Encode(true);

        sol = solver.Solve(true);
        if (sol.size() == 0) break;
    
        for (auto it = not_refuted_bool.begin(); it != not_refuted_bool.end(); ) {
            if (mapping.Retrieve(icsp.GetBoolVar(it->first), sol) != it->second) {
                it = not_refuted_bool.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = not_refuted_int.begin(); it != not_refuted_int.end(); ) {
            if (mapping.Retrieve(icsp.GetIntVar(it->first), sol) != it->second) {
                it = not_refuted_int.erase(it);
            } else {
                ++it;
            }
        }
    }
    std::cout << "sat" << std::endl;
    for (auto& p : not_refuted_bool) {
        std::cout << p.first << ' ' << (p.second ? "true" : "false") << std::endl;
    }
    for (auto& p : not_refuted_int) {
        std::cout << p.first << ' ' << p.second << std::endl;
    }
}

void FindAnswer(CSP& csp) {
    ICSP icsp;
    icsp.LoadVars(csp);

    Converter conv(icsp);
    conv.Convert(csp);

    icsp.Propagate();

    Simplifier simp(icsp);
    simp.Simplify();

    if (icsp.IsUnsatisfiable()) {
        std::cout << "s UNSATISFIABLE" << std::endl;
        return;
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
        for (auto& vars : csp.BoolVars()) {
            std::cout << "a " << vars.first << '\t' << (mapping.Retrieve(icsp.GetBoolVar(vars.first), sol) ? "true" : "false") << std::endl; 
        }
        for (auto& vars : csp.IntVars()) {
            std::cout << "a " << vars.first << '\t' << mapping.Retrieve(icsp.GetIntVar(vars.first), sol) << std::endl; 
        }
        std::cout << "a" << std::endl;
    }
}

int main() {
    bool has_answer_key;
    std::vector<std::string> answer_keys;
    CSP input = InputCSP(has_answer_key, answer_keys);

    if (has_answer_key) {
        SolveIrrefutably(input, answer_keys);
    } else {
        FindAnswer(input);
    }

    return 0;
}

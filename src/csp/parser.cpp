#include "csp/parser.h"

#include <memory>
#include <map>
#include <string>

#include "csp/expr.h"
#include "csp/var.h"

namespace csugar {

namespace {

const std::map<std::string, ExprType> kTokenToExprType = {
    {"not", kNot},
    {"!", kNot},
    {"and", kAnd},
    {"&&", kAnd},
    {"or", kOr},
    {"||", kOr},
    {"imp", kImp},
    {"=>", kImp},
    {"xor", kXor},
    {"iff", kIff},
    {"eq", kEq},
    {"=", kEq},
    {"==", kEq},
    {"ne", kNe},
    {"!=", kNe},
    {"le", kLe},
    {"<=", kLe},
    {"lt", kLt},
    {"<", kLt},
    {"ge", kGe},
    {">=", kGe},
    {"gt", kGt},
    {">", kGt},
    {"add", kAdd},
    {"+", kAdd},
    {"sub", kSub},
    {"-", kSub},
    {"if", kIf},
    {"alldifferent", kAllDifferent},
};

std::string NextToken(const std::string& s, int& p) {
    std::string ret;
    while (true) {
        if (p >= s.size() || s[p] == ' ' || s[p] == ')') {
            break;
        }
        ret.push_back(s[p++]);
    }
    return ret;
}
std::shared_ptr<Expr> ParserSub(const std::string& s, CSP& csp, int& p,
                                const std::map<std::string, CSPBoolVar>& bool_map,
                                const std::map<std::string, CSPIntVar>& int_map) {
    if (p >= s.size()) {
        throw ParseError("unexpected end of line");
    }
    if (s[p] == '(') {
        ++p;
        std::string name = NextToken(s, p);
        if (kTokenToExprType.count(name) == 0) {
            throw ParseError("unknown operator");
        }
        ExprType type = kTokenToExprType.at(name);
        std::vector<std::shared_ptr<Expr>> children;
        while (true) {
            if (s[p] == ')') {
                ++p;
                break;
            } else if (s[p] == ' ') {
                ++p;
                auto sub = ParserSub(s, csp, p, bool_map, int_map);
                children.push_back(sub);
            } else {
                throw ParseError("unexpected token");
            }
        }
        return std::make_shared<Expr>(type, children); // TODO
    } else {
        std::string v = NextToken(s, p);

        if ('0' <= v[0] && v[0] <= '9') {
            // integer
            return Expr::ConstInt(std::stoi(v));
        } else if (v == "true") {
            return Expr::ConstBool(true);
        } else if (v == "false") {
            return Expr::ConstBool(false);
        } else {
            if (bool_map.count(v) > 0) {
                return Expr::VarBool(bool_map.at(v));
            } else if (int_map.count(v) > 0) {
                return Expr::VarInt(int_map.at(v));
            } else {
                throw ParseError("unknown variable name");
            }
        }
    }
}
}

std::shared_ptr<Expr> StringToExpr(const std::string& s, CSP& csp,
                                   const std::map<std::string, CSPBoolVar>& bool_map,
                                   const std::map<std::string, CSPIntVar>& int_map) {
    int p = 0;
    return ParserSub(s, csp, p, bool_map, int_map);
}

}

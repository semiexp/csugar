#include "csp/parser.h"

#include <memory>
#include <map>
#include <string>

#include "csp/expr.h"

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
std::shared_ptr<Expr> ParserSub(const std::string& s, CSP& csp, int& p) {
    if (p >= s.size()) {
        throw ParseError("unexpected end of line");
    }
    if (s[p] == '(') {
        ++p;
        std::string name = NextToken(s, p);
        ExprType type = kTokenToExprType.at(name);
        std::vector<std::shared_ptr<Expr>> children;
        while (true) {
            if (s[p] == ')') {
                ++p;
                break;
            } else if (s[p] == ' ') {
                ++p;
                auto sub = ParserSub(s, csp, p);
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
            if (csp.HasBoolVar(v)) {
                return Expr::VarBool(v);
            } else if (csp.HasIntVar(v)) {
                return Expr::VarInt(v);
            } else {
                throw ParseError("unknown variable name");
            }
        }
    }
}
}

std::shared_ptr<Expr> StringToExpr(const std::string& s, CSP& csp) {
    int p = 0;
    return ParserSub(s, csp, p);
}

}

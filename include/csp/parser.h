#pragma once

#include <memory>
#include <string>
#include <exception>
#include <map>

#include "csp/csp.h"
#include "csp/expr.h"
#include "csp/var.h"

namespace csugar {

class ParseError : public std::runtime_error {
public:
    ParseError(const char* str) : std::runtime_error(str) {}
    ParseError(const std::string& str) : std::runtime_error(str) {}
};

std::shared_ptr<Expr> StringToExpr(const std::string& s, CSP& csp,
                                   const std::map<std::string, CSPBoolVar>& bool_map,
                                   const std::map<std::string, CSPIntVar>& int_map);

}

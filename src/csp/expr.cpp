#include "csp/expr.h"

namespace csugar {

bool Expr::Equal(const std::shared_ptr<Expr>& lhs, const std::shared_ptr<Expr>& rhs) {
    if (lhs->type() != rhs->type()) return false;
    if (lhs->type() == kConstantBool) {
        return lhs->AsConstantBool() == rhs->AsConstantBool();
    } else if (lhs->type() == kConstantInt) {
        return lhs->AsConstantInt() == rhs->AsConstantInt();
    } else if (lhs->type() == kVariableBool) {
        return lhs->AsBoolVar().id() == rhs->AsBoolVar().id();
    } else if (lhs->type() == kVariableInt) {
        return lhs->AsIntVar().id() == rhs->AsIntVar().id();
    } else if (lhs->type() == kInternalVariableInt) {
        return lhs->AsInternalIntVar() == rhs->AsInternalIntVar();
    } else if (lhs->type() == kInternalVariableBool) {
        return lhs->AsInternalBoolVar() == rhs->AsInternalBoolVar();
    }
    if (lhs->size() != rhs->size()) return false;
    for (int i = 0; i < lhs->size(); ++i) {
        if (!Equal((*lhs)[i], (*rhs)[i])) return false;
    }
    return true;
}

}

#include "csp/expr.h"

namespace csugar {

bool Expr::Equal(const std::shared_ptr<Expr>& lhs, const std::shared_ptr<Expr>& rhs) {
    if (lhs->type() != rhs->type()) return false;
    if (lhs->type() == kConstantBool) {
        return lhs->AsConstantBool() == rhs->AsConstantBool();
    } else if (lhs->type() == kConstantInt) {
        return lhs->AsConstantInt() == rhs->AsConstantInt();
    } else if (lhs->type() == kVariableBool || lhs->type() == kVariableInt) {
        return lhs->VariableName() == rhs->VariableName();
    }
    if (lhs->size() != rhs->size()) return false;
    for (int i = 0; i < lhs->size(); ++i) {
        if (!Equal((*lhs)[i], (*rhs)[i])) return false;
    }
    return true;
}

}

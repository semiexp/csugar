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

uint64_t Expr::Hash() const {
    uint64_t hash_children;

    switch (type_) {
    case kConstantBool:
        hash_children = (int)AsConstantBool();
        break;
    case kConstantInt:
        hash_children = AsConstantInt();
        break;
    case kVariableBool:
        hash_children = AsBoolVar().id();
        break;
    case kVariableInt:
        hash_children = AsIntVar().id();
        break;
    case kInternalVariableBool:
        hash_children = (uint64_t)AsInternalBoolVar().get();
        break;
    case kInternalVariableInt:
        hash_children = (uint64_t)AsInternalIntVar().get();
        break;
    default:
        hash_children = 0;
        for (int i = 0; i < size(); ++i) {
            hash_children += (1234567890123456ULL * i + 789012345678ULL) * children_[i]->Hash();
        }
    }
    hash_children += type() * 456789012345678901ULL;
    return hash_children;
}

}

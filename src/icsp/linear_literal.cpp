#include "icsp/linear_literal.h"

#include <string>
#include <algorithm>
#include <memory>

#include "common/util.h"
#include "common/domain.h"

namespace csugar {

bool LinearLiteral::IsUnsatisfiable() const {
    auto domain = sum_.GetDomain();
    switch (op_) {
        case kLitEq: return domain->GetLowerBound() > 0 || domain->GetUpperBound() < 0;
        case kLitNe: return domain->GetLowerBound() == 0 && domain->GetUpperBound() == 0;
        case kLitLe: return domain->GetLowerBound() > 0;
        case kLitGe: return domain->GetUpperBound() < 0;
    }
}
bool LinearLiteral::IsValid() const {
    auto domain = sum_.GetDomain();
    if (domain->GetLowerBound() > domain->GetUpperBound()) return false;
    switch (op_) {
        case kLitEq: return domain->GetLowerBound() == 0 && domain->GetUpperBound() == 0;
        case kLitNe: return domain->GetLowerBound() > 0 || domain->GetUpperBound() < 0;
        case kLitLe: return domain->GetUpperBound() <= 0;
        case kLitGe: return domain->GetLowerBound() >= 0;
    }
}
bool LinearLiteral::IsSimple() const {
    // TODO: log encoding?
    if (op_ == kLitGe || op_  == kLitLe) {
        return sum_.IsSimple();
    } else {
        return sum_.size() == 0;
    }
}
std::string LinearLiteral::str() const {
    std::string ret = sum_.str();
    switch (op_) {
        case kLitEq: ret += "==0"; break;
        case kLitNe: ret += "!=0"; break;
        case kLitGe: ret += ">=0"; break;
        case kLitLe: ret += "<=0"; break;
    }
    return ret;
}
std::set<std::shared_ptr<ICSPIntVar>> LinearLiteral::IntVars() const {
    std::set<std::shared_ptr<ICSPIntVar>> ret;
    for (auto coef_ : sum_.GetCoefs()) {
        ret.insert(coef_.first);
    }
    return ret;
}
std::pair<int, int> LinearLiteral::GetBound(std::shared_ptr<ICSPIntVar> v) const {
    if (op_ == kLitNe) {
        return { v->domain()->GetLowerBound(), v->domain()->GetUpperBound() };
    }
    auto& domain = v->domain();
    int lb = domain->GetLowerBound(), ub = domain->GetUpperBound();
    int a = sum_.GetCoef(v);

    auto [lb_other, ub_other] = sum_.GetDomainRangeExcept(v);

    if (op_ == kLitLe || op_ == kLitEq) {
        if (a > 0) ub = std::min(ub, FloorDiv(-lb_other, a));
        else lb = std::max(lb, CeilDiv(lb_other, -a));
    }
    if (op_ == kLitGe || op_ == kLitEq) {
        if (a > 0) lb = std::max(lb, CeilDiv(-ub_other, a));
        else ub = std::min(ub, FloorDiv(ub_other, -a));
    }
    return { lb, ub };
}

}

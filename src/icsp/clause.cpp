#include "icsp/clause.h"

#include <string>

#include "common/domain.h"

namespace csugar {

bool Clause::IsSimple() const {
    int n_complex = 0;
    for (int i = 0; i < size(); ++i) {
        if (!(*this)[i]->IsSimple()) ++n_complex;
    }
    return n_complex <= 1;
}
std::string Clause::str() const {
    std::string ret("[");
    for (int i = 0; i < size(); ++i) {
        if (i != 0) ret.push_back(' ');
        ret += (*this)[i]->str();
    }
    ret.push_back(']');
    return ret;
}
std::set<std::shared_ptr<IntVar>> Clause::GetCommonIntVars() const {
    if (size() == 0) return {};
    std::set<std::shared_ptr<IntVar>> ret = literals_[0]->IntVars();
    for (int i = 1; i < size(); ++i) {
        auto tmp = literals_[i]->IntVars();
        for (auto it = ret.begin(); it != ret.end(); ) {
            if (tmp.count(*it) == 0) it = ret.erase(it);
            else ++it;
        }
    }
    return ret;
}
bool Clause::IsValid() const {
    for (int i = 0; i < size(); ++i) {
        if (literals_[i]->IsValid()) return true;
    }
    return false;
}
DomainBoundingResult Clause::Propagate() {
    if (size() == 0) return kNoUpdate;
    DomainBoundingResult ret = kNoUpdate;
    for (auto var : GetCommonIntVars()) {
        std::pair<int, int> bound = literals_[0]->GetBound(var);
        for (int i = 1; i < literals_.size(); ++i) {
            std::pair<int, int> bound_tmp = literals_[i]->GetBound(var);
            bound.first = std::min(bound.first, bound_tmp.first);
            bound.second = std::max(bound.second, bound_tmp.second);
        }
        DomainBoundingResult res = var->Bound(bound.first, bound.second);
        ret = std::max(ret, res);
    }
    return ret;
}
bool Clause::RemoveFalsefood() {
    std::vector<std::shared_ptr<Literal>> literals_new;
    for (auto& lit : literals_) {
        if (!lit->IsUnsatisfiable()) {
            literals_new.push_back(lit);
        }
    }
    bool ret = literals_.size() != literals_new.size();
    literals_.swap(literals_new);
    return ret;
}
}

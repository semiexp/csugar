#include "common/enumerative_domain.h"

#include <memory>
#include <set>
#include <vector>

namespace csugar {

EnumerativeDomain::EnumerativeDomain(int lb, int ub) {
    for (int i = lb; i <= ub; ++i) {
        domain_.insert(i);
    }
}
int EnumerativeDomain::GetLowerBound() const {
    return *domain_.begin();
}
int EnumerativeDomain::GetUpperBound() const {
    return *domain_.rbegin();
}
int EnumerativeDomain::size() const {
    return domain_.size();
}
std::vector<int> EnumerativeDomain::Enumerate() const {
    std::vector<int> ret;
    ret.insert(ret.end(), domain_.begin(), domain_.end());
    return ret;
}
std::unique_ptr<Domain> EnumerativeDomain::Add(const std::unique_ptr<Domain>& other) const {
    std::set<int> domain;
    for (int x : domain_) {
        for (int y : other->Enumerate()) {
            domain.insert(x + y);
        }
    }
    return std::make_unique<EnumerativeDomain>(std::move(domain));
}
std::unique_ptr<Domain> EnumerativeDomain::Sub(const std::unique_ptr<Domain>& other) const {
    std::set<int> domain;
    for (int x : domain_) {
        for (int y : other->Enumerate()) {
            domain.insert(x - y);
        }
    }
    return std::make_unique<EnumerativeDomain>(std::move(domain));
}
std::unique_ptr<Domain> EnumerativeDomain::Mul(int other) const {
    std::set<int> domain;
    for (int x : domain_) {
        domain.insert(x * other);
    }
    return std::make_unique<EnumerativeDomain>(std::move(domain));
}
std::unique_ptr<Domain> EnumerativeDomain::Cup(const std::unique_ptr<Domain>& other) const {
    std::set<int> domain = domain_;
    for (int x : other->Enumerate()) {
        domain.insert(x);
    }
    return std::make_unique<EnumerativeDomain>(std::move(domain));
}
DomainBoundingResult EnumerativeDomain::Bound(int lb, int ub) {
    bool removed = false;
    for (auto it = domain_.begin(); it != domain_.end(); ) {
        if (!(lb <= *it && *it <= ub)) {
            removed = true;
            it = domain_.erase(it);
        } else ++it;
    }
    if (domain_.size() == 0) return kEmptyDomain;
    return removed ? kUpdate : kNoUpdate;
}

}

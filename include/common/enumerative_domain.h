#pragma once

#include <vector>
#include <set>

#include "common/domain.h"

namespace csugar {

class EnumerativeDomain : public Domain {
public:
    EnumerativeDomain(int n) : EnumerativeDomain(n, n) {}
    EnumerativeDomain(int lb, int ub);
    EnumerativeDomain(const std::set<int>& domain) : domain_(domain) {}
    EnumerativeDomain(std::set<int>&& domain) : domain_(std::move(domain)) {}
    ~EnumerativeDomain() override {}

    int GetLowerBound() const override;
    int GetUpperBound() const override;
    int size() const override;
    std::vector<int> Enumerate() const override;

    std::unique_ptr<Domain> Add(const std::unique_ptr<Domain>& other) const override;
    std::unique_ptr<Domain> Sub(const std::unique_ptr<Domain>& other) const override;
    std::unique_ptr<Domain> Mul(int other) const override;
    std::unique_ptr<Domain> Cup(const std::unique_ptr<Domain>& other) const override;
    DomainBoundingResult Bound(int lb, int ub) override;

    virtual std::unique_ptr<Domain> clone() const override;

private:
    std::set<int> domain_;
};

}

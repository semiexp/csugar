#pragma once

#include <vector>

#include "common/domain.h"

namespace csugar {

class IntervalDomain : public Domain {
public:
    IntervalDomain(int n) : lb_(n), ub_(n) {}
    IntervalDomain(int lb, int ub) : lb_(lb), ub_(ub) {}
    ~IntervalDomain() override;

    int GetLowerBound() const override;
    int GetUpperBound() const override;
    int size() const override;
    std::vector<int> Enumerate() const override;

    std::unique_ptr<Domain> Add(const std::unique_ptr<Domain>& other) const override;
    std::unique_ptr<Domain> Sub(const std::unique_ptr<Domain>& other) const override;
    std::unique_ptr<Domain> Mul(int other) const override;
    std::unique_ptr<Domain> Cup(const std::unique_ptr<Domain>& other) const override;
    DomainBoundingResult Bound(int lb, int ub) override;

private:
    int lb_, ub_;
};

}

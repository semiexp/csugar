#pragma once

#include "common/domain.h"

namespace csugar
{

class IntervalDomain : public Domain
{
public:
    IntervalDomain(int n) : lb_(n), ub_(n) {}
    IntervalDomain(int lb, int ub) : lb_(lb), ub_(ub) {}

    int GetLowerBound() const { return lb_; }
    int GetUpperBound() const { return ub_; }

    virtual std::shared_ptr<Domain> Add(std::shared_ptr<Domain> other) const;
    virtual std::shared_ptr<Domain> Sub(std::shared_ptr<Domain> other) const;
private:
    int lb_, ub_;
};

}

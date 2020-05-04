#include "common/interval_domain.h"

#include <memory>

namespace csugar
{

// TODO: refined domain?
std::shared_ptr<Domain> IntervalDomain::Add(std::shared_ptr<Domain> other) const
{
    return std::make_shared<IntervalDomain>(IntervalDomain(lb_ + other->GetLowerBound(), ub_ + other->GetUpperBound()));
}
std::shared_ptr<Domain> IntervalDomain::Sub(std::shared_ptr<Domain> other) const
{
    return std::make_shared<IntervalDomain>(IntervalDomain(lb_ - other->GetUpperBound(), ub_ - other->GetLowerBound()));
}

}

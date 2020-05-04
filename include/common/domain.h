#pragma once

#include <memory>

namespace csugar
{

class Domain
{
public:
    Domain() {}
    virtual ~Domain() {}

    virtual int GetLowerBound() const;
    virtual int GetUpperBound() const;

    virtual std::shared_ptr<Domain> Add(std::shared_ptr<Domain> other) const;
    virtual std::shared_ptr<Domain> Sub(std::shared_ptr<Domain> other) const;
};

}

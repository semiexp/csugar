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

    virtual std::unique_ptr<Domain> Add(std::unique_ptr<Domain>& other) const;
    virtual std::unique_ptr<Domain> Sub(std::unique_ptr<Domain>& other) const;
    virtual std::unique_ptr<Domain> Cup(std::unique_ptr<Domain>& other) const;
};

}

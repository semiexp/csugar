#pragma once

#include <memory>

namespace csugar {

class Domain {
public:
    Domain() {}
    virtual ~Domain() {}

    virtual int GetLowerBound() const = 0;
    virtual int GetUpperBound() const = 0;

    virtual std::unique_ptr<Domain> Add(const std::unique_ptr<Domain>& other) const = 0;
    virtual std::unique_ptr<Domain> Sub(const std::unique_ptr<Domain>& other) const = 0;
    virtual std::unique_ptr<Domain> Mul(int other) const = 0;
    virtual std::unique_ptr<Domain> Cup(const std::unique_ptr<Domain>& other) const = 0;
};

}

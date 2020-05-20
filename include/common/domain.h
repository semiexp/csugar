#pragma once

#include <memory>
#include <vector>

namespace csugar {

enum DomainBoundingResult {
    kNoUpdate = 0, kUpdate = 1, kEmptyDomain = 2
};

class Domain {
public:
    Domain() {}
    virtual ~Domain() {}

    virtual int GetLowerBound() const = 0;
    virtual int GetUpperBound() const = 0;
    virtual int size() const = 0;
    virtual std::vector<int> Enumerate() const = 0;

    virtual std::unique_ptr<Domain> Add(const std::unique_ptr<Domain>& other) const = 0;
    virtual std::unique_ptr<Domain> Sub(const std::unique_ptr<Domain>& other) const = 0;
    virtual std::unique_ptr<Domain> Mul(int other) const = 0;
    virtual std::unique_ptr<Domain> Cup(const std::unique_ptr<Domain>& other) const = 0;
    virtual DomainBoundingResult Bound(int lb, int ub) = 0;

    virtual std::unique_ptr<Domain> clone() const = 0;
};

}

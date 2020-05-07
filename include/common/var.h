#pragma once

#include <set>
#include <string>
#include <memory>

#include "common/domain.h"

namespace csugar {

class BoolVar {
public:
    BoolVar(std::string name) : name_(name) {}

    std::string name() const { return name_; }

private:
    std::string name_;
};

class IntVar {
public:
    IntVar(std::unique_ptr<Domain> &&domain, std::string name) : domain_(std::move(domain)), name_(name) {}

    std::unique_ptr<Domain>& domain() { return domain_; }
    const std::unique_ptr<Domain>& domain() const { return domain_; }
    std::string name() const { return name_; }
    DomainBoundingResult Bound(int lb, int ub) { return domain_->Bound(lb, ub); }

private:
    std::unique_ptr<Domain> domain_;
    std::string name_;
};

}

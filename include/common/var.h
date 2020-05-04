#pragma once

#include <set>
#include <string>
#include <memory>

#include "common/domain.h"

namespace csugar {

class BoolVar
{
public:
    BoolVar(std::string name) : name_(name) {}

    std::string name() const { return name_; }

private:
    std::string name_;
};

class IntVar
{
public:
    IntVar(std::shared_ptr<Domain> domain, std::string name) : domain_(domain), name_(name) {}

    std::shared_ptr<Domain> domain() { return domain_; }
    std::shared_ptr<const Domain> domain() const { return domain_; }
    std::string name() const { return name_; }

private:
    std::shared_ptr<Domain> domain_;
    std::string name_;
};

}

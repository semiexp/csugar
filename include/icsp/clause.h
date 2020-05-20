#pragma once

#include <vector>
#include <memory>
#include <string>
#include <set>

#include "common/domain.h"
#include "icsp/literal.h"
#include "icsp/var.h"

namespace csugar {

class Clause {
public:
    Clause() {}
    Clause(std::shared_ptr<Literal> lit) { literals_.push_back(lit); }

    int size() const { return literals_.size(); }
    std::shared_ptr<Literal> operator[](int i) const { return literals_[i]; }

    void Add(std::shared_ptr<Literal> p) {
        literals_.push_back(p);
    }

    bool IsSimple() const;

    std::string str() const;

    std::set<std::shared_ptr<ICSPIntVar>> GetCommonIntVars() const;
    bool IsValid() const;
    DomainBoundingResult Propagate();
    bool RemoveFalsefood();

private:
    std::vector<std::shared_ptr<Literal>> literals_;
};

}

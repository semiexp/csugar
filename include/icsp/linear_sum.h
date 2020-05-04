#pragma once

#include <memory>
#include <map>

#include "common/var.h"
#include "common/domain.h"

namespace csugar
{

class LinearSum {
public:
    LinearSum(int b = 0) : coef_(), b_(b) {}
    LinearSum(std::shared_ptr<IntVar> v) : coef_(), b_(0) {
        coef_.insert({v, 1});
    }

    std::unique_ptr<Domain> GetDomain();
    void Factorize();

    LinearSum& operator+=(const LinearSum& rhs);
    LinearSum& operator-=(const LinearSum& rhs);
    LinearSum& operator*=(int rhs);

private:
    std::map<std::shared_ptr<IntVar>, int> coef_;
    int b_;
};

}

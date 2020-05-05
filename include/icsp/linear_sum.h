#pragma once

#include <memory>
#include <map>
#include <string>

#include "common/var.h"
#include "common/domain.h"

namespace csugar {

class LinearSum {
public:
    LinearSum(int b = 0) : coef_(), b_(b) {}
    LinearSum(std::shared_ptr<IntVar> v) : coef_(), b_(0) {
        coef_.insert({v, 1});
    }

    int size() const { return coef_.size(); }
    std::unique_ptr<Domain> GetDomain();
    void Factorize();
    bool IsSimple() const { return coef_.size() <= 1; }

    const std::map<std::shared_ptr<IntVar>, int>& GetCoef() const { return coef_; }
    std::map<std::shared_ptr<IntVar>, int>& GetCoef() { return coef_; }
    int GetB() const { return b_; }

    LinearSum& operator+=(const LinearSum& rhs) {
        WeightedAdd(rhs, 1);
        return *this;
    }
    LinearSum& operator-=(const LinearSum& rhs) {
        WeightedAdd(rhs, -1);
        return *this;
    }
    LinearSum& operator*=(int rhs);

    std::string str() const;

private:
    void WeightedAdd(const LinearSum& other, int w);

    std::map<std::shared_ptr<IntVar>, int> coef_;
    int b_;
};

}

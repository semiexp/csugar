#include "icsp/linear_sum.h"

#include <string>
#include <cstdlib>
#include <memory>

#include "common/interval_domain.h"

int gcd(int a, int b)
{
    a = abs(a);
    b = abs(b);
    while (b > 0) {
        int tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

namespace csugar {

std::unique_ptr<Domain> LinearSum::GetDomainExcept(std::shared_ptr<IntVar> except) const {
    std::unique_ptr<Domain> ret = std::make_unique<IntervalDomain>(b_);
    for (auto& it : coef_) {
        if (it.first != except) {
            ret = ret->Add(it.first->domain()->Mul(it.second));
        }
    }
    return ret;
}
void LinearSum::Factorize() {
    int g = b_;
    for (auto it : coef_) {
        g = gcd(g, it.second);
    }
    if (g == 0) return;

    b_ /= g;
    for (auto& it : coef_) {
        it.second /= g;
    }
}
LinearSum& LinearSum::operator*=(int rhs) {
    if (rhs == 0) {
        b_ = 0;
        coef_.clear();
    } else {
        b_ *= rhs;
        for (auto& it : coef_) {
            it.second *= rhs;
        }
    }
    return *this;
}
std::string LinearSum::str() const {
    // For reproducibility, variables are sorted by their names
    std::map<std::string, int> coef_sorted;
    for (auto& it : coef_) {
        coef_sorted.insert({it.first->name(), it.second});
    }
    std::string ret;
    for (auto& it : coef_sorted) {
        ret += it.first;
        ret.push_back('*');
        ret += std::to_string(it.second);
        ret.push_back('+');
    }
    ret += std::to_string(b_);
    return ret;
}
void LinearSum::WeightedAdd(const LinearSum& other, int w) {
    b_ += other.b_ * w;
    for (auto& it : other.coef_) {
        if (coef_.count(it.first) > 0) {
            coef_[it.first] += it.second * w;
            if (coef_[it.first] == 0) {
                coef_.erase(it.first);
            }
        } else {
            coef_[it.first] = it.second * w;
        }
    }
}

}

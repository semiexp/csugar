#include "icsp/linear_sum.h"

#include <algorithm>
#include <string>
#include <cstdlib>
#include <memory>

#include "csp/expr.h"
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

std::unique_ptr<Domain> LinearSum::GetDomainExcept(std::shared_ptr<ICSPIntVar> except) const {
    std::unique_ptr<Domain> ret = std::make_unique<IntervalDomain>(b_);
    for (auto& it : coef_) {
        if (it.first != except) {
            ret = ret->Add(it.first->domain()->Mul(it.second));
        }
    }
    return ret;
}
int LinearSum::GetExpectedDomainSize(bool exclude_largest, int threshold) const {
    int ret = 1;
    auto vars_sorted = GetVariablesSorted();
    for (int i = 0; i + (exclude_largest ? 1 : 0) < vars_sorted.size(); ++i) {
       int s = vars_sorted[i]->domain()->size();
       if (ret <= threshold / s) {
           ret *= s;
       } else {
           return threshold;
       }
    }
    return ret;
}
void LinearSum::Factorize() {
    int g = Factor();
    if (g != 0) Divide(g);
}
std::vector<std::shared_ptr<ICSPIntVar>> LinearSum::GetVariablesSorted() const {
    std::vector<std::shared_ptr<ICSPIntVar>> ret;
    for (auto& p : coef_) {
        ret.push_back(p.first);
    }
    std::sort(ret.begin(), ret.end(), [](std::shared_ptr<ICSPIntVar>& l, std::shared_ptr<ICSPIntVar>& r) {
        return l->domain()->size() < r->domain()->size();
    });
    return ret;
}
std::vector<LinearSum> LinearSum::Split(int s) const {
    std::vector<LinearSum> ret;
    for (int i = 0; i < s; ++i) {
        ret.push_back(LinearSum(0));
    }
    auto vars = GetVariablesSorted();
    for (int i = 0; i < vars.size(); ++i) {
        ret[i % s].coef_[vars[i]] = coef_.at(vars[i]);
    }
    return ret;
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
    // For reproducibility, variables are sorted by ids
    std::map<int, int> coef_sorted;
    for (auto& it : coef_) {
        coef_sorted.insert({it.first->id(), it.second});
    }
    std::string ret;
    for (auto& it : coef_sorted) {
        ret += std::string("i") + std::to_string(it.first);
        ret.push_back('*');
        ret += std::to_string(it.second);
        ret.push_back('+');
    }
    ret += std::to_string(b_);
    return ret;
}
int LinearSum::Factor() const {
    int g = abs(b_);
    for (auto it : coef_) {
        g = gcd(g, it.second);
    }
    return g;
}
void LinearSum::Divide(int v) {
    b_ /= v;
    for (auto& p : coef_) {
        p.second /= v;
    }
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
std::shared_ptr<Expr> LinearSum::ToExpr() {
    std::vector<std::shared_ptr<Expr>> ch;
    ch.push_back(Expr::ConstInt(b_));
    for (auto& p : coef_) {
        ch.push_back(Expr::Make(kMul, { Expr::InternalVarInt(p.first), Expr::ConstInt(p.second) }));
    }
    return std::make_shared<Expr>(kAdd, ch);
}

}

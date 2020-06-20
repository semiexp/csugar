#pragma once

#include <memory>
#include <map>
#include <string>

#include "csp/expr.h"
#include "icsp/var.h"
#include "common/domain.h"

namespace csugar {

class LinearSum {
public:
    LinearSum(int b = 0) : coef_(), b_(b) {}
    LinearSum(std::shared_ptr<ICSPIntVar> v) : coef_(), b_(0) {
        coef_.insert({v, 1});
    }

    int size() const { return coef_.size(); }
    std::unique_ptr<Domain> GetDomain() const { return GetDomainExcept(std::shared_ptr<ICSPIntVar>(nullptr)); }
    std::unique_ptr<Domain> GetDomainExcept(std::shared_ptr<ICSPIntVar> except) const;
    int GetExpectedDomainSize(bool exclude_largest, int threshold = 1048576) const;
    void Factorize();
    std::vector<std::shared_ptr<ICSPIntVar>> GetVariablesSorted() const;
    std::vector<LinearSum> Split(int s) const;
    bool IsSimple() const { return coef_.size() <= 1; }

    const int GetCoef(std::shared_ptr<ICSPIntVar> var) const { return coef_.at(var); }
    std::vector<std::pair<std::shared_ptr<ICSPIntVar>, int>> GetCoefs() const;
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
    int Factor() const;
    void Divide(int v);

    std::string str() const;

    std::shared_ptr<Expr> ToExpr();

private:
    struct ICSPIntVarCompare
    {
        inline bool operator()(const std::shared_ptr<ICSPIntVar>& a, const std::shared_ptr<ICSPIntVar>& b) const
        {
            return a->id() < b->id();
        }
    };
    void WeightedAdd(const LinearSum& other, int w);

    std::map<std::shared_ptr<ICSPIntVar>, int, ICSPIntVarCompare> coef_;
    int b_;
};

}

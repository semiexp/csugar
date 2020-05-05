#pragma once

namespace csugar {

class SATLit {
public:
    constexpr SATLit(int v, bool negative = false) : v_(v), negative_(negative) {}
    int GetVariable() const { return v_; }
    bool IsNegative() const { return negative_; }

    SATLit operator!() const { return SATLit(v_, !negative_); }

    bool operator==(const SATLit& rhs) {
        return v_ == rhs.v_ && negative_ == rhs.negative_;
    }
    bool operator!=(const SATLit& rhs) {
        return !(v_ == rhs.v_ && negative_ == rhs.negative_);
    }

private:
    int v_;
    bool negative_;
};

}

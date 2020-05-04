#pragma once

#include <vector>
#include <memory>

namespace csugar {

enum ExprType
{
    kConstantInt,
    kConstantBool,
    kVariableInt,
    kVariableBool,
    kNot,
    kAnd,
    kOr,
    kImp,
    kXor,
    kIff,
    kEq,
    kNe,
    kLe,
    kLt,
    kGe,
    kGt,
    kNeg,
    kAbs,
    kAdd,
    kSub,
    kMul,
    kDiv,
    kMod,
    kPow,
    kMin,
    kMax,
    kIf,
};

class Expr
{
public:
private:
    ExprType type_;
    union {
        std::vector<std::shared_ptr<Expr>> children_;
        int constant_int_;
        bool constant_bool_;
        int variable_id_;
    };
};

}

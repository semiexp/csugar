#pragma once

#include <vector>
#include <memory>
#include <string>
#include <initializer_list>

#include "csp/var.h"
#include "icsp/var.h"

namespace csugar {

enum ExprType {
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
    kAllDifferent,
    kInternalVariableBool,
    kInternalVariableInt,
    kGraphActiveVerticesConnected,
};

class Expr {
public:
    Expr(ExprType type) : type_(type) {}
    Expr(ExprType type, std::shared_ptr<Expr> expr) : type_(type), children_{expr} {}
    Expr(ExprType type, const std::vector<std::shared_ptr<Expr>>& exprs) : type_(type), children_(exprs) {}
    Expr(ExprType type, std::initializer_list<std::shared_ptr<Expr>> il) : type_(type), children_(il) {}

    ExprType type() const { return type_; }
    int size() const { return children_.size(); }
    std::shared_ptr<Expr> operator[](int i) { return children_[i]; }
    int AsConstantInt() const { return constant_int_; }
    bool AsConstantBool() const { return constant_bool_; }
    const CSPBoolVar& AsBoolVar() const { return bool_var_; }
    const CSPIntVar& AsIntVar() const { return int_var_; }
    const std::shared_ptr<ICSPBoolVar>& AsInternalBoolVar() const { return icsp_bool_var_; }
    const std::shared_ptr<ICSPIntVar>& AsInternalIntVar() const { return icsp_int_var_; }

    bool IsLogical() const {
        return type_ == kConstantBool || type_ == kVariableBool || type_ == kNot || type_ == kAnd
            || type_ == kOr || type_ == kImp || type_ == kXor || type_ == kIff;
    }
    bool IsComparison() const {
        return type_ == kEq || type_ == kNe || type_ == kLe || type_ == kLt || type_ == kGe || type_ == kGt;
    }

    static std::shared_ptr<Expr> Make(ExprType type) {
        return std::make_shared<Expr>(type);
    }
    static std::shared_ptr<Expr> Make(ExprType type, std::shared_ptr<Expr> expr) {
        return std::make_shared<Expr>(type, expr);
    }
    static std::shared_ptr<Expr> Make(ExprType type, std::initializer_list<std::shared_ptr<Expr>> il) {
        return std::make_shared<Expr>(type, il);
    }
    static std::shared_ptr<Expr> Not(std::shared_ptr<Expr> expr) {
        return Expr::Make(kNot, expr);
    }
    static std::shared_ptr<Expr> And(std::shared_ptr<Expr> a, std::shared_ptr<Expr> b) {
        return Expr::Make(kAnd, {a, b});
    }
    static std::shared_ptr<Expr> Or(std::shared_ptr<Expr> a, std::shared_ptr<Expr> b) {
        return Expr::Make(kOr, {a, b});
    }
    static std::shared_ptr<Expr> Eq(std::shared_ptr<Expr> a, std::shared_ptr<Expr> b) {
        return Expr::Make(kEq, {a, b});
    }
    static std::shared_ptr<Expr> ConstInt(int i) {
        auto ret = Expr::Make(kConstantInt);
        ret->constant_int_ = i;
        return ret;
    }
    static std::shared_ptr<Expr> ConstBool(bool b) {
        auto ret = Expr::Make(kConstantBool);
        ret->constant_bool_ = b;
        return ret;
    }
    static std::shared_ptr<Expr> VarBool(const CSPBoolVar& var) {
        auto ret = Expr::Make(kVariableBool);
        ret->bool_var_ = var;
        return ret;
    }
    static std::shared_ptr<Expr> VarInt(const CSPIntVar& var) {
        auto ret = Expr::Make(kVariableInt);
        ret->int_var_ = var;
        return ret;
    }
    static std::shared_ptr<Expr> InternalVarBool(const std::shared_ptr<ICSPBoolVar>& var) {
        auto ret = Expr::Make(kInternalVariableBool);
        ret->icsp_bool_var_ = var;
        return ret;
    }
    static std::shared_ptr<Expr> InternalVarInt(const std::shared_ptr<ICSPIntVar>& var) {
        auto ret = Expr::Make(kInternalVariableInt);
        ret->icsp_int_var_ = var;
        return ret;
    }
    static bool Equal(const std::shared_ptr<Expr>& lhs, const std::shared_ptr<Expr>& rhs);
private:
    ExprType type_;
    std::vector<std::shared_ptr<Expr>> children_;
    CSPBoolVar bool_var_;
    CSPIntVar int_var_;
    std::shared_ptr<ICSPBoolVar> icsp_bool_var_;
    std::shared_ptr<ICSPIntVar> icsp_int_var_;
    union {
        int constant_int_;
        bool constant_bool_;
    };
};

}

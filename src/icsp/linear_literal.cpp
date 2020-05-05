#include "icsp/linear_literal.h"

#include <string>

namespace csugar {

bool LinearLiteral::IsSimple() const {
    // TODO: log encoding?
    if (op_ == kLitGe || op_  == kLitLe) {
        return sum_.IsSimple();
    } else {
        return sum_.size() == 0;
    }
}
std::string LinearLiteral::str() const {
    std::string ret = sum_.str();
    switch (op_) {
        case kLitEq: ret += "==0"; break;
        case kLitNe: ret += "!=0"; break;
        case kLitGe: ret += ">=0"; break;
        case kLitLe: ret += "<=0"; break;
    }
    return ret;
}

}

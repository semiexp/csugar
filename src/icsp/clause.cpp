#include "icsp/clause.h"

#include <string>

namespace csugar {

std::string Clause::str() const {
    std::string ret("[");
    for (int i = 0; i < size(); ++i) {
        if (i != 0) ret.push_back(' ');
        ret += (*this)[i]->str();
    }
    ret.push_back(']');
    return ret;
}

}
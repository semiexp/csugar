#include "icsp/clause.h"

#include <string>

namespace csugar {

bool Clause::IsSimple() const {
    int n_complex = 0;
    for (int i = 0; i < size(); ++i) {
        if (!(*this)[i]->IsSimple()) ++n_complex;
    }
    return n_complex <= 1;
}
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
#pragma once

#include <string>

namespace csugar {

class Literal {
public:
    Literal() {}
    virtual ~Literal() {}

    virtual std::string str() const = 0;
};

}

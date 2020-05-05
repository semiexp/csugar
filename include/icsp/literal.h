#pragma once

#include <string>

namespace csugar {

class Literal {
public:
    Literal() {}
    virtual ~Literal() {}

    virtual bool IsSimple() const = 0;
    virtual bool IsValid() const = 0;

    virtual std::string str() const = 0;
};

}

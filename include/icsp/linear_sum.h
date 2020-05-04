#pragma once

#include <memory>
#include <vector>

#include "common/var.h"

namespace csugar
{

class LinearSum {
private:
    std::vector<std::shared_ptr<IntVar>> var_;
    std::vector<int> coef_;
    int b_;
};

}

#include "common/util.h"

namespace csugar {

int CeilDiv(int a, int b) {
    if (b < 0) {
        a = -a;
        b = -b;
    }
    if (a >= 0) {
        return (a + b - 1) / b;
    } else {
        return a / b;
    }
}
int FloorDiv(int a, int b) {
    if (b < 0) {
        a = -a;
        b = -b;
    }
    if (a >= 0) {
        return a / b;
    } else {
        return (a - b + 1) / b;
    }
}
}

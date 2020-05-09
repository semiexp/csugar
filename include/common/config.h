#pragma once

namespace csugar {

struct Config {
    int max_linearsum_size = 1024;
    bool incremental_propagation = true;
    bool normalize_linearsum = true;
    bool reduce_arity = true;
};

}

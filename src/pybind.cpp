#include <pybind11/pybind11.h>

#include <sstream>

#include "main.h"

namespace py = pybind11;

std::string PythonSolver(const std::string& input) {
    std::istringstream iss(input);
    std::ostringstream oss;

    bool has_answer_key;
    std::vector<std::string> answer_keys;
    IntegratedCSPSolver solver;
    InputCSP(solver, has_answer_key, answer_keys, iss);

    if (has_answer_key) {
        SolveIrrefutably(solver, answer_keys, oss);
    } else {
        FindAnswer(solver, oss);
    }

    return oss.str();
}

PYBIND11_MODULE(pycsugar, m) {
    m.def("solver", &PythonSolver);
}

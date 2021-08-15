#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include <map>

#include "integrated/integrated.h"

using namespace csugar;

void InputCSP(IntegratedCSPSolver& solver, bool& has_answer_key, std::vector<std::string>& answer_keys, std::istream &input);

void SolveIrrefutably(IntegratedCSPSolver& solver, std::vector<std::string>& answer_keys, std::ostream &output);
void FindAnswer(IntegratedCSPSolver& solver, std::ostream &output);

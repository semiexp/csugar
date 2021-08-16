#pragma once

#ifdef USE_GLUCOSE
#include "core/Solver.h"
#include "core/Constraint.h"
#define BACKEND Glucose
#else
#include "minisat/core/Solver.h"
#include "minisat/core/Constraint.h"
#define BACKEND Minisat
#endif

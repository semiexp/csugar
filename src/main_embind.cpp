#include <emscripten/bind.h>

#include "integrated/integrated.h"

using namespace emscripten;
using namespace csugar;

namespace {

class SolverWrapper {
public:
    SolverWrapper() {}

    void Add(const std::string& in) { solver_.Parse(in); }
    val FindAnswer() {
        CSPAnswer answer = solver_.Solve();

        val ret = val::object();
        ret.set("is_sat", answer.IsSat());
        if (!answer.IsSat()) return ret;

        auto bool_vars = solver_.BoolVars();
        for (auto& var : bool_vars) {
            ret.set(var, val(answer.GetBool(var)));
        }
        auto int_vars = solver_.IntVars();
        for (auto& var : int_vars) {
            ret.set(var, val(answer.GetInt(var)));
        }
        return ret;
    }
    val Solve(val answer_keys) {
        int n_answer_keys = answer_keys["length"].as<int>();
        std::vector<std::string> answer_key_vec;
        for (int i = 0; i < n_answer_keys; ++i) {
            answer_key_vec.push_back(answer_keys[i].as<std::string>());
        }

        solver_.SetTargetVars(answer_key_vec);
        CSPAnswer answer = solver_.Solve();

        val ret = val::object();
        ret.set("is_sat", answer.IsSat());
        if (!answer.IsSat()) return ret;

        std::map<std::string, bool> not_refuted_bool;
        std::map<std::string, int> not_refuted_int;
        for (auto& name : answer_key_vec) {
            if (solver_.HasBoolVar(name)) {
                not_refuted_bool.insert({name, answer.GetBool(name)});
            } else if (solver_.HasIntVar(name)) {
                not_refuted_int.insert({name, answer.GetInt(name)});
            } else {
                ret.set("error", true);
                return ret;
            }
        }
        while (true) {
            std::vector<std::shared_ptr<Expr>> refuting_exprs;
            for (auto& p : not_refuted_bool) {
                refuting_exprs.push_back(Expr::Make(kXor, {Expr::VarBool(solver_.GetBoolVar(p.first)), Expr::ConstBool(p.second)}));
            }
            for (auto& p : not_refuted_int) {
                refuting_exprs.push_back(Expr::Make(kNe, {Expr::VarInt(solver_.GetIntVar(p.first)), Expr::ConstInt(p.second)}));
            }
            solver_.AddConstraint(std::make_shared<Expr>(kOr, refuting_exprs));
            
            answer = solver_.Solve();
            if (!answer.IsSat()) break;
        
            for (auto it = not_refuted_bool.begin(); it != not_refuted_bool.end(); ) {
                if (answer.GetBool(it->first) != it->second) {
                    it = not_refuted_bool.erase(it);
                } else {
                    ++it;
                }
            }
            for (auto it = not_refuted_int.begin(); it != not_refuted_int.end(); ) {
                if (answer.GetInt(it->first) != it->second) {
                    it = not_refuted_int.erase(it);
                } else {
                    ++it;
                }
            }
        }
        for (auto& p : not_refuted_bool) {
            ret.set(p.first, val(p.second));
        }
        for (auto& p : not_refuted_int) {
            ret.set(p.first, val(p.second));
        }
        return ret;
    }

private:
    csugar::IntegratedCSPSolver solver_;
};

}

EMSCRIPTEN_BINDINGS(csugar) {
    class_<SolverWrapper>("CSugarSolver")
      .constructor<>()
      .function("Add", &SolverWrapper::Add)
      .function("FindAnswer", &SolverWrapper::FindAnswer)
      .function("Solve", &SolverWrapper::Solve)
      ;
}

#include "integrated/integrated.h"

#include <iostream>

#include "common/enumerative_domain.h"
#include "csp/parser.h"

namespace {
std::vector<std::string> Tokenize(const std::string& s) {
    std::vector<std::string> ret;
    std::string token;
    for (int i = 0; i < s.size(); ++i) {
        if (s[i] == ' ') {
            if (token.size() > 0) ret.push_back(token);
            token.clear();
        } else {
            token.push_back(s[i]);
        }
    }
    if (token.size() > 0) ret.push_back(token);
    return ret;
}
std::vector<std::string> SplitDepthOneExpr(const std::string& s) {
    return Tokenize(s.substr(1, (int)s.size() - 2));
}

bool IsBoolDefinition(const std::string& s) {
    return s.substr(0, 5) == "(bool";
}
bool IsIntDefinition(const std::string& s) {
    return s.substr(0, 4) == "(int";
}
}

namespace csugar {

IntegratedCSPSolver::IntegratedCSPSolver() :
    bool_var_map_(), int_var_map_(), csp_(new CSP()), icsp_(nullptr), sat_(nullptr), conv_(nullptr), simplifier_(nullptr), encoder_(nullptr) {}

void IntegratedCSPSolver::Parse(const std::string& in) {
    if (IsBoolDefinition(in)) {
        auto toks = SplitDepthOneExpr(in);
        if (toks.size() != 2) {
            // TODO
            std::cerr << "invalid bool token" << std::endl;
            exit(1);
        }
        MakeBoolVar(toks[1]);
    } else if (IsIntDefinition(in)) {
        auto toks = SplitDepthOneExpr(in);
        if (toks.size() != 4) {
            // TODO
            std::cerr << "invalid int token" << std::endl;
            exit(1);
        }
        MakeIntVar(toks[1], std::make_unique<EnumerativeDomain>(std::stoi(toks[2]), std::stoi(toks[3])));
    } else {
        AddConstraint(StringToExpr(in, bool_var_map_, int_var_map_));
    }
}
std::vector<std::string> IntegratedCSPSolver::BoolVars() const {
    std::vector<std::string> ret;
    for (auto v : bool_var_map_) {
        ret.push_back(v.first);
    }
    return ret;
}
std::vector<std::string> IntegratedCSPSolver::IntVars() const {
    std::vector<std::string> ret;
    for (auto v : int_var_map_) {
        ret.push_back(v.first);
    }
    return ret;
}
CSPBoolVar IntegratedCSPSolver::MakeBoolVar(const std::string& name) {
    if (bool_var_map_.count(name) > 0 || int_var_map_.count(name) > 0) {
        // TODO
        abort();
    }
    CSPBoolVar ret = csp_->MakeBoolVar();
    bool_var_map_[name] = ret;
    return ret;
}
CSPIntVar IntegratedCSPSolver::MakeIntVar(const std::string& name, std::unique_ptr<Domain>&& domain) {
    if (bool_var_map_.count(name) > 0 || int_var_map_.count(name) > 0) {
        // TODO
        abort();
    }
    CSPIntVar ret = csp_->MakeIntVar(std::move(domain));
    int_var_map_[name] = ret;
    return ret;
}
void IntegratedCSPSolver::AddConstraint(std::shared_ptr<Expr> expr) {
    csp_->AddExpr(expr);
}
CSPAnswer IntegratedCSPSolver::Solve() {
    bool incremental = static_cast<bool>(icsp_);

    if (!incremental) {
        icsp_ = std::make_unique<ICSP>();
        sat_ = std::make_unique<SAT>();
        mapping_ = std::make_unique<Mapping>(*sat_);
        conv_ = std::make_unique<Converter>(*csp_, *icsp_);
        simplifier_ = std::make_unique<Simplifier>(*icsp_);
        encoder_ = std::make_unique<Encoder>(*icsp_, *sat_, *mapping_);
        solver_ = std::make_unique<Solver>(*sat_);
    }

    conv_->Convert(incremental);
    if (!incremental) icsp_->Propagate();
    simplifier_->Simplify(incremental);

    if (icsp_->IsUnsatisfiable()) {
        return CSPAnswer();
    }

    encoder_->Encode(incremental);
    auto answer_raw = solver_->Solve(incremental);
    if (answer_raw.size() == 0) {
        return CSPAnswer();
    }

    std::map<std::string, bool> bool_ans;
    std::map<std::string, int> int_ans;
    if (target_vars_.has_value()) {
        const std::vector<std::string>& target_vars = target_vars_.value();
        for (auto& v : target_vars) {
            if (HasBoolVar(v)) {
                bool_ans[v] = mapping_->Retrieve(conv_->ConvertBoolVar(GetBoolVar(v)), answer_raw);
            } else if (HasIntVar(v)) {
                int_ans[v] = mapping_->Retrieve(conv_->ConvertIntVar(GetIntVar(v)), answer_raw);
            } else {
                abort(); // TODO
            }
        }
    } else {
        for (auto& v : bool_var_map_) {
            bool_ans[v.first] = mapping_->Retrieve(conv_->ConvertBoolVar(v.second), answer_raw);
        }
        for (auto& v : int_var_map_) {
            int_ans[v.first] = mapping_->Retrieve(conv_->ConvertIntVar(v.second), answer_raw);
        }
    }

    return CSPAnswer(std::move(bool_ans), std::move(int_ans));
}
}

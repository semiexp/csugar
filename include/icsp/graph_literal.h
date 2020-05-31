#pragma once

#include "icsp/literal.h"
#include "icsp/var.h"

#include <vector>
#include <algorithm>

namespace csugar {

enum GraphLiteralType {
    kActiveVerticesConnectedLiteral
};

class GraphLiteral : public Literal {
public:
    GraphLiteral(const std::vector<std::shared_ptr<ICSPBoolVar>>& vars,
                 const std::vector<bool>& is_negative,
                 const std::vector<std::pair<int, int>>& edges,
                 GraphLiteralType kind) : vars_(vars), is_negative_(is_negative), edges_(edges), kind_(kind) {}
    ~GraphLiteral() = default;

    bool IsSimple() const override { return false; }
    bool IsValid() const override { return false; }
    bool IsUnsatisfiable() const override { return false; }

    std::string str() const override { return "<graph>"; }
    
    std::set<std::shared_ptr<ICSPIntVar>> IntVars() const override { return {}; }
    std::pair<int, int> GetBound(std::shared_ptr<ICSPIntVar> v) const override { return { v->domain()->GetLowerBound(), v->domain()->GetUpperBound() }; }

    const std::vector<std::shared_ptr<ICSPBoolVar>>& vars() const { return vars_; }
    const std::vector<bool>& is_negative() const { return is_negative_; }
    const std::vector<std::pair<int, int>>& edges() const { return edges_; }
    GraphLiteralType kind() const { return kind_; }

private:
    std::vector<std::shared_ptr<ICSPBoolVar>> vars_;
    std::vector<bool> is_negative_;
    std::vector<std::pair<int, int>> edges_;
    GraphLiteralType kind_;
};

}

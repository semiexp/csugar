#include "conv/converter.h"
#include "common/var.h"
#include "icsp/bool_literal.h"

namespace csugar
{

std::vector<Clause> Converter::ConvertConstraint(std::shared_ptr<Expr> expr, bool negative)
{
    std::vector<Clause> clauses;

    while (true) {
        if (expr->type() == kConstantBool) {
            bool actual = (expr->AsConstantBool() != negative);
            if (actual) {
                // satisfied; do nothing
            } else {
                // unsatisfied
                clauses.push_back(Clause());
            }
            break;
        } else if (expr->type() == kVariableBool) {
            clauses.push_back(Clause(std::make_shared<BoolLiteral>(icsp_.GetBoolVar(expr->VariableName()), negative)));
            break;
        } else if (expr->IsLogical()) {
            if (expr->type() == kNot) {
                expr = (*expr)[0];
                negative = !negative;
                continue;
            }
            expr = ConvertLogical(expr, negative, clauses);
            if (!expr) break;
        } else if (expr->IsComparison()) {
            expr = ConvertComparison(expr, negative, clauses);
            if (!expr) break;
        }
    }
    return clauses;
}
std::shared_ptr<Expr> Converter::ConvertLogical(std::shared_ptr<Expr> expr, bool negative, std::vector<Clause> &clauses)
{
    if (expr->type() == kImp) {
        return Expr::Or(Expr::Not((*expr)[0]), (*expr)[1]);
    } else if (expr->type() == kXor) {
        // TODO: this seems inefficient
        return Expr::And(Expr::Or((*expr)[0], (*expr)[1]), Expr::Or(Expr::Not((*expr)[0]), Expr::Not((*expr)[1])));
    } else if (expr->type() == kIff) {
        // TODO: this seems inefficient
        return Expr::And(Expr::Or((*expr)[0], Expr::Not((*expr)[1])), Expr::Or(Expr::Not((*expr)[0]), (*expr)[1]));
    } else if ((expr->type() == kAnd && !negative) || (expr->type() == kOr && negative)) {
        for (int i = 0; i < expr->size(); ++i) {
            auto clauses_sub = ConvertConstraint((*expr)[i], negative);
            clauses.insert(clauses.end(), clauses_sub.begin(), clauses_sub.end());
        }
        return std::shared_ptr<Expr>(nullptr);
    } else if ((expr->type() == kAnd && negative) || (expr->type() == kOr && !negative)) {
        auto clauses_sub = ConvertDisj(expr, negative);
        clauses.insert(clauses.end(), clauses_sub.begin(), clauses_sub.end());
    } else {
        // TODO: error
    }
}
std::vector<Clause> Converter::ConvertDisj(std::shared_ptr<Expr> expr, bool negative)
{
    std::vector<Clause> clauses;
    if (expr->size() == 0) {
        clauses.push_back(Clause());
    } else if (expr->size() == 1) {
        clauses = ConvertConstraint((*expr)[0], negative);
    } else {
        Clause aux_clause;
        for (int i = 0; i < expr->size(); ++i) {
            auto clauses_sub = ConvertConstraint((*expr)[i], negative);
            if (clauses_sub.size() == 0) {
                return clauses_sub;
            } else if (clauses_sub.size() == 1) {
                Clause &c = clauses_sub[0];
                for (int j = 0; j < c.size(); ++j) {
                    aux_clause.Add(c[j]);
                }
            } else {
                auto v = icsp_.AuxiliaryBoolVar();
                auto v0 = std::make_shared<BoolLiteral>(v, false);
                auto v1 = std::make_shared<BoolLiteral>(v, true);
                aux_clause.Add(v0);

                // TODO: EQUIV_TRANSLATION?
                for (int j = 0; j < clauses_sub.size(); ++j) {
                    auto c = clauses_sub[j];
                    c.Add(v1);
                    clauses.push_back(c);
                }
            }
        }
    }
    return clauses;
}

}

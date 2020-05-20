#pragma once

#include <set>
#include <string>
#include <memory>

#include "common/domain.h"

namespace csugar {

class ICSPBoolVar {
public:
    ICSPBoolVar(int id) : id_(id) {}

    int id() const { return id_; }

private:
    int id_;
};

class ICSPIntVar {
public:
    ICSPIntVar(std::unique_ptr<Domain> &&domain, int id) : domain_(std::move(domain)), id_(id), encoded_(false) {}

    std::unique_ptr<Domain>& domain() { return domain_; }
    const std::unique_ptr<Domain>& domain() const { return domain_; }
    int id() const { return id_; }

    DomainBoundingResult Bound(int lb, int ub) {
        if (encoded_) return kNoUpdate;
        else return domain_->Bound(lb, ub);
    }
    bool IsEncoded() const { return encoded_; }
    void SetEncoded() { encoded_ = true; }

private:
    std::unique_ptr<Domain> domain_;
    int id_;
    bool encoded_;
};

}

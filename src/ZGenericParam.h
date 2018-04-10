#pragma once

#include <string>
#include "ZType.h"

class ZGenericParam : public ZType {
public:
    ZGenericParam(std::string& name) : _name(name) {

    }

    llvm::Type* toLlvmType() override {
        throw std::exception("Type parameter cannot be converted to LLVM type");
    }

    std::string& getName() override {
        return _name;
    }

    std::string& toString() override {
        return _name;
    }

    bool isEqual(ZType& other) override {
        return &other == this;
    }

private:
    std::string& _name;
};

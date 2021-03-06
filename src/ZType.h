﻿#pragma once
#include <string>
#include <llvm/IR/Type.h>

class ZType {
public:
    virtual llvm::Type* toLlvmType() = 0;

    virtual std::string& getName() = 0;

    virtual std::string& toString() = 0;

    virtual bool isEqual(ZType& other) = 0;

    virtual bool canBeCastedTo(ZType* other);

    virtual ZType* copyStem();

    virtual bool isDelayed() {
        return false;
    }

    bool hasDelayedParts() {
        if (isDelayed())
            return true;

        for (auto param : *_typeParams)
            if (param->hasDelayedParts())
                return true;

        return false;
    }

    std::vector<ZType*>* getTypeParams() {
        return _typeParams;
    }

    void setTypeParam(int n, ZType* type) {
        (*_typeParams)[n] = type;
    }

    bool containsGenerics();
protected:
    std::vector<ZType*>* _typeParams = new std::vector<ZType*>();
};

#pragma once
#include "ZType.h"

class GenericResolutionChain {
public:
    ZType* resolve(ZType* type) {
        int depth = _chain.size() - 1;
        while (type->containsGenerics()) {
            type = _chain[depth]->resolve(type);
            --depth;
        }

        return type;
    }

    void push(SymbolRef* ref) {
        _chain.push_back(ref);
    }

    void pop() {
        _chain.pop_back();
    }

private:
    vector<SymbolRef*> _chain;
};

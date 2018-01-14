#pragma once
#include "ZType.h"
#include "ZStructType.h"
#include "SymbolRef.h"

class ZClassType : public ZStructType {
public:
    ZClassType(std::string* name, std::vector<ZArg*>* members, SymbolRef* scopeRef, std::vector<ZGenericParam*>* genericDefs = nullptr)
        : ZStructType(name, members, genericDefs) {
        this->scopeRef = scopeRef;
    }

    SymbolRef* scopeRef;
};

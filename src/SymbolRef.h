#pragma once
#include "ZFuncType.h"
#include "ZArrayType.h"
#include "ZDelayedType.h"

class SymbolEntry;
class SymbolScope;

class SymbolRef {
public:
    SymbolRef(SymbolScope* storage, int id, SymbolEntry* entry = nullptr);

    SymbolEntry* getEntry();

    std::vector<SymbolEntry*>* findSymbolDef(std::string& name, bool onlyCurrentScope = false);

    ZType* findTypeDef(std::string& name);

    ZType* resolve(ZType* type);

    ZType* findTypeOrDelayed(std::string& name);

    ZType* resolveIfDelayed(ZType* type);

    void addResolution(ZGenericParam* param, ZType* arg);

    SymbolScope* getStorage();

private:
    SymbolScope* _storage;
    int _id;
    SymbolEntry* _entry;
};

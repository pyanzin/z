#pragma once
#include "SymbolStorage.h"

class SymbolEntry;

class SymbolRef {
public:
    SymbolRef(SymbolScope* storage, int id) {
        _storage = storage;
        _id = id;
    }

	SymbolEntry* getEntry() {
        return _storage->getSymbolEntries()[_id];
    }

    SymbolEntry* findSymbolDef(std::string& name, bool onlyCurrentScope = false) {
        return _storage->findSymbol(_id, name, onlyCurrentScope);
    }

	ZType* findTypeDef(std::string& name) {
		return _storage->findType(_id, name);
	}

    ZType* resolve(ZType* type) {
        ZGenericParam* gen = dynamic_cast<ZGenericParam*>(type);
        if (!gen)
            return type;

        return _storage->resolveGeneric(gen);
    }

	void addResolution(ZGenericParam* param, ZType* arg) {
		_storage->addTypeArgument(param, arg);
    }
private:
    SymbolScope* _storage;
    int _id;
};

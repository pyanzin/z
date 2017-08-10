#pragma once
#include "SymbolStorage.h"

class SymbolEntry;

class SymbolRef {
public:
    SymbolRef(SymbolStorage* storage, int id) {
        _storage = storage;
        _id = id;
    }

	SymbolEntry* getEntry() {
        return _storage->getEntries()[_id];
    }

    SymbolEntry* findDefinedBefore(std::string& name, bool onlyCurrentScope = false) {
        return _storage->findDefinedBefore(_id, name, onlyCurrentScope);
    }
private:
    SymbolStorage* _storage;
    int _id;
};

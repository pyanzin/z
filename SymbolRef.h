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

    SymbolEntry* findDefinedBefore(std::string& name) {
        return _storage->findDefinedBefore(_id, name);
    }
private:
    SymbolStorage* _storage;
    int _id;
};

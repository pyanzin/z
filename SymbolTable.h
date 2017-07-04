#pragma once
#include <string>
#include "ZBasicTypes.h"
#include "SymbolStorage.h"
#include "SymbolRef.h"

class SymbolTable {
public:
    SymbolTable() {
        storage = new SymbolStorage();
    }
    void enter() {
        storage = storage->makeChild();
    };
    void exit() {
        storage = storage->getParent();
    }

	SymbolRef* add(ZType* type, std::string* name) {
        int id = storage->add(new SymbolEntry(type, *name));
        return new SymbolRef(storage, id);
    }

	SymbolRef* makeRef() {
		int id = storage->incrementNumber();
		return new SymbolRef(storage, id);
    }

private:
    SymbolStorage* storage;
};

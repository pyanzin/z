#pragma once
#include <string>
#include "ZBasicType.h"
#include "SymbolScope.h"
#include "SymbolRef.h"

class SymbolTable {
public:
	SymbolTable() {
		storage = new SymbolScope();
	}
	void enter() {
		storage = storage->makeChild();
	};
	void exit() {
		storage = storage->getParent();
	}

	SymbolRef* addSymbol(ZType* type, std::string* name) {
		int id = storage->add(new SymbolEntry(type, *name));
		return new SymbolRef(storage, id);
	}

	SymbolRef* addType(ZType* type) {
		int id = storage->add(type);
		return new SymbolRef(storage, id);
	}

	SymbolRef* makeRef() {
		int id = storage->incrementNumber();
		return new SymbolRef(storage, id);
    }

private:
    SymbolScope* storage;
};

#pragma once
#include <string>
#include "ZBasicType.h"
#include "SymbolScope.h"
#include "SymbolRef.h"

class SymbolTable {
public:
	SymbolTable() {
		storage = new SymbolScope(nullptr, false);
	}

	void enter(bool ordered = true) {
		storage = storage->makeChild(ordered);
	}

	void exit() {
		storage = storage->getParent();
	}

	SymbolRef* addSymbol(ZType* type, std::string* name, SymbolType symbolType) {
		return storage->add(*name, type, symbolType);
	}

	SymbolRef* addType(ZType* type) {
		int id = storage->add(type);
		return new SymbolRef(storage, id);
	}

	SymbolRef* makeRef() {
		int id = storage->incrementOrdinal();
		return new SymbolRef(storage, id);
    }

private:
    SymbolScope* storage;
};

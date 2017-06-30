#pragma once
#include <string>
#include "ZBasicTypes.h"
#include "SymbolStorage.h"

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

	void add(ZType* type, std::string& name, const std::string& value = nullptr) {
        storage->add(new SymbolEntry(type, name, value));
    }

private:
    SymbolStorage* storage;
};

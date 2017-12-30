#include "SymbolScope.h"
#include "SymbolRef.h"
#include "SymbolEntry.h"

SymbolRef* SymbolScope::makeRef() {
	int id = incrementOrdinal();
	return new SymbolRef(this, id);
}

SymbolRef* SymbolScope::add(std::string name, ZType* type) {
    int id = _ordinal++;
    if (_symbolStorage.find(name) == _symbolStorage.end())
        _symbolStorage[name] = new std::vector<SymbolEntry*>();

    auto entry = new SymbolEntry(type, name, id);
    _symbolStorage[name]->push_back(entry);
    return new SymbolRef(this, id, entry);
}

std::vector<SymbolEntry*>* SymbolScope::findSymbol(int id, std::string& name, bool onlyCurrentScope) {
    std::vector<SymbolEntry*>* result = new std::vector<SymbolEntry*>();

    SymbolScope* scope = this;
    do {
        auto storage = scope->_symbolStorage;

        if (storage.find(name) == storage.end()) {
            id = scope->_parentNumber;
            continue;
        }

        std::vector<SymbolEntry*>* entries = storage[name];

        for (auto entry : *entries) {
            if (entry->getOrdinal() < id || !scope->isOrdered())
                result->push_back(entry);
        }
        id = scope->_parentNumber;
    } while (scope = onlyCurrentScope ? nullptr : scope->getParent());

    return result;
}
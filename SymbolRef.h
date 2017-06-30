#pragma once

class SymbolEntry;
class SymbolStorage;

class SymbolRef {
public:
    SymbolRef(SymbolStorage* storage, int entryNumber) {
        _storage = storage;
        _entryNumber = entryNumber;
    }

	SymbolEntry* getEntry() {
	    //_storage->
    }

private:
    SymbolStorage* _storage;
    int _entryNumber;
};

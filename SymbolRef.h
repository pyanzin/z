#pragma once

class SymbolEntry;
class SymbolStorage;

class SymbolRef {
public:
    SymbolRef(SymbolStorage* storage, int entryNumber) {
        _storage = storage;
        _entryNumber = entryNumber;
    }

private:
    SymbolStorage* _storage;
    int _entryNumber;
};

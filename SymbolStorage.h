#pragma once
#include <map>
#include "SymbolEntry.h"

class SymbolStorage {
public:
    SymbolStorage(SymbolStorage* parent = nullptr) {
        _parent = parent;
        _number = 0;
        if (_parent)
            _parentNumber = _parent->incrementNumber();
        else
            _parentNumber = -1;            
    }

    SymbolStorage* getParent() {
        return _parent;
    }

    void add(SymbolEntry* entry) {
        entries[_number++] = entry;
    };

    SymbolStorage* makeChild() {
        return new SymbolStorage(this);
    }
private:
    int _number;
    SymbolStorage* _parent;
    int _parentNumber;

    int incrementNumber() {
        return _number++;
    }

    std::map<int, SymbolEntry*> entries;
};

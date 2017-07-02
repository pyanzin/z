﻿#pragma once
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

    int add(SymbolEntry* entry) {
        int id = _number++;
        _entries[id] = entry;
        return id;
    };

    SymbolStorage* makeChild() {
        return new SymbolStorage(this);
    }

    SymbolEntry* findDefinedBefore(int id, std::string& name) {
        SymbolStorage* storage = this;
        do {
            auto entries = this->_entries;
            for (int i = 0; i < id; ++i)
                if (name == entries[i]->getName())
                    return entries[i];
        } while (storage->getParent());

        return nullptr;
    }

    std::map<int, SymbolEntry*>& getEntries() {
        return _entries;
    }
private:
    int _number;
    SymbolStorage* _parent;
    int _parentNumber;

    int incrementNumber() {
        return _number++;
    }

    std::map<int, SymbolEntry*> _entries;
};

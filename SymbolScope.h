﻿#pragma once
#include <map>
#include "SymbolEntry.h"
#include "ZGenericParam.h"

class SymbolScope {
public:
	SymbolScope(SymbolScope* parent = nullptr) {
        _parent = parent;
        _number = 0;
        if (_parent)
            _parentNumber = _parent->incrementNumber();
        else
            _parentNumber = -1;            
    }

    SymbolScope* getParent() {
        return _parent;
    }

    int add(SymbolEntry* symbol) {
        int id = _number++;
		_symbolEntries[id] = symbol;
        return id;
    }

	int add(ZType* type) {
		int id = _number++;
		_typeEntries[id] = type;
		return id;
	}

    SymbolScope* makeChild() {
        return new SymbolScope(this);
    }

	bool isTopLevel() {
		return getParent() == nullptr;
    }

    SymbolEntry* findSymbol(int id, std::string& name, bool onlyCurrentScope = false) {
        SymbolScope* storage = this;
        do {
			auto entries = storage->_symbolEntries;
			for (auto entry : entries) {
				if (entry.first >= id && !storage->isTopLevel())
					break;

				if (name == entry.second->getName())
					return entry.second;
			}
			id = storage->_parentNumber;
        } while (storage = onlyCurrentScope ? nullptr : storage->getParent());

        return nullptr;
    }

	ZType* findType(int id, std::string& name) {
		SymbolScope* storage = this;
		do {
			auto entries = storage->_typeEntries;
			for (auto entry : entries) {
				if (entry.first >= id && !storage->isTopLevel())
					break;

				if (name == entry.second->getName())
					return entry.second;
			}
			id = storage->_parentNumber;
		} while (storage = storage->getParent());

		return nullptr;
    }

	ZType* resolveGeneric(ZGenericParam* param) {
		SymbolScope* storage = this;
		do {
			auto entries = storage->_typeArguments;
			for (auto entry : entries) {				
				if (param->isEqual(*entry.first))
					return entry.second;
			}
		} while (storage = storage->getParent());

		return Unknown;
	}

	void addTypeArgument(ZGenericParam* param, ZType* arg) {
		_typeArguments[param] = arg;
    }

	ZType* resolve(ZGenericParam* param) {
		return _typeArguments[param];
    }

    std::map<int, SymbolEntry*>& getSymbolEntries() {
		return _symbolEntries;
    }

	int incrementNumber() {
		return _number++;
	}

	bool isDefined(int id, ZType* type) {
		SymbolScope* storage = this;
		do {
			auto entries = storage->_typeEntries;
			for (auto entry : entries) {
				if (entry.first >= id && !storage->isTopLevel())
					break;

				if (type == entry.second)
					return true;
			}
			id = storage->_parentNumber;
		} while (storage = storage->getParent());

		return false;
	}
private:
    int _number;
    SymbolScope* _parent;
    int _parentNumber;

    std::map<int, SymbolEntry*> _symbolEntries;
	std::map<int, ZType*> _typeEntries;
	std::map<ZGenericParam*, ZType*> _typeArguments;
};
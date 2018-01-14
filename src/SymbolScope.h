#pragma once
#include <map>
#include "ZGenericParam.h"
#include "SymbolRef.h"
#include "SymbolType.h"

class SymbolScope {
public:
	SymbolScope(SymbolScope* parent = nullptr, bool ordered = true) {
        _parent = parent;
        _ordered = ordered;
        _ordinal = 0;
        if (_parent)
            _parentNumber = _parent->incrementOrdinal();
        else
            _parentNumber = -1;            
    }

    SymbolScope* getParent() {
        return _parent;
    }

    SymbolRef* add(std::string name, ZType* type, SymbolType symbolType, bool outOfOrder = false);

	int add(ZType* type) {
		int id = _ordinal++;
		_typeEntries[id] = type;
		return id;
	}

    SymbolScope* makeChild(bool ordered = true) {
        return new SymbolScope(this, ordered);
    }

    bool isOrdered() {
        return _ordered;
    }

    std::vector<SymbolEntry*>* findSymbol(int id, std::string& name, bool onlyCurrentScope = false);

	ZType* findType(int id, std::string& name) {
		SymbolScope* storage = this;
		do {
			auto entries = storage->_typeEntries;
			for (auto entry : entries) {
				if (entry.first >= id && !storage->isOrdered())
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

	int incrementOrdinal() {
		return _ordinal++;
	}

	bool isDefined(int id, ZType* type) {
		SymbolScope* storage = this;
		do {
			auto entries = storage->_typeEntries;
			for (auto entry : entries) {
				if (entry.first >= id && !storage->isOrdered())
					break;

				if (type == entry.second)
					return true;
			}
			id = storage->_parentNumber;
		} while (storage = storage->getParent());

		return false;
	}

	SymbolRef* makeRef();

private:
    int _ordinal;
    SymbolScope* _parent;
    bool _ordered;
    int _parentNumber;

    std::map<std::string, std::vector<SymbolEntry*>*> _symbolStorage;
	std::map<int, ZType*> _typeEntries;
	std::map<ZGenericParam*, ZType*> _typeArguments;
};

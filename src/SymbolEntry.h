#pragma once
#include <string>
#include "ZBasicType.h"
#include "SymbolType.h"

class SymbolEntry {
public:
	SymbolEntry(ZType* type, std::string& name, int ordinal, SymbolType symbolType) : _name(name) {
        _type = type;
        _ordinal = ordinal;
        this->symbolType = symbolType;
    }

    ZType* getType() {
        return _type;
	}

    std::string& getName() {
        return _name;
	}

	void setType(ZType* type) {
		_type = type;
	};

    int getOrdinal() {
        return _ordinal;
    }

    SymbolType symbolType;
private:
	ZType* _type;
    std::string& _name;
    int _ordinal;
};

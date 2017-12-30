#pragma once
#include <string>
#include "ZBasicType.h"

class SymbolEntry {
public:
	SymbolEntry(ZType* type, std::string& name, int ordinal) : _name(name) {
        _type = type;
        _ordinal = ordinal;
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

private:
	ZType* _type;
    std::string& _name;
    int _ordinal;
};

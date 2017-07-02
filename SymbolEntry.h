#pragma once
#include <string>
#include "ZBasicTypes.h"

class SymbolEntry {
public:
	SymbolEntry(ZType* type, std::string& name) : _name(name) {
        _type = type;
    }

    ZType* getType() {
        return _type;
	}

    std::string& getName() {
        return _name;
	}

private:
	ZType* _type;
    std::string& _name;
};

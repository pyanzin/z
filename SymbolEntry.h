#pragma once
#include <string>
#include "ZBasicType.h"

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

	void setType(ZType* type) {
		_type = type;
	};
private:
	ZType* _type;
    std::string& _name;
};

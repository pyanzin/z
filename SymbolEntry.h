#pragma once
#include <string>
#include "ZBasicTypes.h"

class SymbolEntry {
public:
    SymbolEntry(BaseTypes type, const std::string& name, const std::string& value = nullptr)
        : _name(name), _value(value){
        _type = type;
    }

private:
    BaseTypes _type;
    const std::string& _name;
    const std::string& _value;
};

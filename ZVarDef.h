#pragma once
#include "ZAst.h"
#include <string>
#include "ZBasicTypes.h"

class ZVarDef : public ZAst {
public:
    ZVarDef(std::string& name, BaseTypes type) : _name(name) {
        _type = type;
    }

    std::string& getName() {
        return _name;
    }
private:
    std::string& _name;
    BaseTypes _type;
};

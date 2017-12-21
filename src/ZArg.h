#pragma once

#include "ZAst.h"
#include <vector>

class ZVisitor;
class ZType;

class ZArg : public ZAst {
public:
    ZArg(ZType* type, std::string* name) {
        _type = type;
        _name = name;
    }

	ZType* getType() {
        return _type;
    }

    void setType(ZType* type) {
        _type = type;
    }

    std::string* getName() {
        return _name;
    }

	virtual void accept(ZVisitor* visitor);
private:
	ZType* _type;
    std::string* _name;
};
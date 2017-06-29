#pragma once

#include "ZBasicTypes.h"
#include "ZAst.h"
#include "ZVisitor.h"

class ZArg : public ZAst {
public:
    ZArg(BaseTypes type, std::string* name)
    {
        _type = type;
        _name = name;
    }

    BaseTypes getType() {
        return _type;
    }

    std::string getName() {
        return *_name;
    }

	virtual void accept(ZVisitor* visitor)
    {
		visitor->visit(this);
    }

private:
    BaseTypes _type;
    std::string* _name;
};

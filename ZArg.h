#ifndef ZARG
#define ZARG

#include "ZBasicTypes.h"
#include "ZAst.h"

class ZArg : public ZAst {
public:
    ZArg(BaseTypes type, char* name)
    {
        _type = type;
        _name = name;
    }

    BaseTypes getType() {
        return _type;
    }

    char* getName() {
        return _name;
    }

	virtual void accept(ZVisitor* visitor)
    {
		visitor->visit(this);
    }

private:
    BaseTypes _type;
    char* _name;
};

#endif
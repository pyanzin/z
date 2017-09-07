#pragma once

#include <vector>
#include "ZExpr.h"
#include "ZBlock.h"

class ZArg;

class ZFunc : public ZExpr {
public:

	ZFunc(std::string* name, ZType* returnType, std::vector<ZArg*>& args, ZBlock* body, bool isExtern = false) : _args(args) {
        _name = name;
        _returnType = returnType;
        _body = body;
		_isExtern = isExtern;
    }

	void accept(ZVisitor* visitor);

	void dump(std::ostream& stream, unsigned depth) override;

	bool isExtern() { return _isExtern; }

//private:
    std::string* _name;
	ZType* _returnType;
    std::vector<ZArg*> _args;
    ZBlock* _body;
	bool _isExtern;
};

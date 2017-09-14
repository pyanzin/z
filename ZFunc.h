#pragma once

#include <vector>
#include "ZExpr.h"
#include "ZBlock.h"
#include "ZBasicTypes.h"
#include "ZArg.h"

class ZArg;

class ZFunc : public ZExpr {
public:

	ZFunc(std::string* name, ZType* returnType, std::vector<ZArg*>& args, ZAst* body, bool isExtern = false) : _args(args) {
        _name = name;
        _returnType = returnType;
        _body = body;
		_isExtern = isExtern;

		std::vector<ZType*>* argTypes = new std::vector<ZType*>();
		for (ZArg* arg : args) {
			argTypes->push_back(arg->getType());
		}

		ZFuncType* funcType = new ZFuncType(returnType, *argTypes);

		setType(funcType);
    }

	void accept(ZVisitor* visitor);

	void dump(std::ostream& stream, unsigned depth) override;

	bool isExtern() { return _isExtern; }

//private:
    std::string* _name;
	ZType* _returnType;
    std::vector<ZArg*> _args;
	ZAst* _body;
	bool _isExtern;
};

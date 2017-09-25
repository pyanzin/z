#pragma once

#include <vector>
#include "ZExpr.h"
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
			adopt(arg);
		}
		ZFuncType* funcType = new ZFuncType(returnType, *argTypes);
		setType(funcType);

		if (body)
			adopt(body);
    }

	void accept(ZVisitor* visitor);

	void replaceChild(ZAst* oldChild, ZAst* newChild) override {
		adopt(newChild);
		if (_body == oldChild) {
			_body = newChild;
			return;
		}

		throw exception("wrong call to replaceChild in ZFunc");
	}

	void dump(std::ostream& stream, unsigned depth) override;

	bool isExtern() { return _isExtern; }

//private:
    std::string* _name;
	ZType* _returnType;
    std::vector<ZArg*> _args;
	ZAst* _body;
	bool _isExtern;
};

#pragma once

#include <vector>
#include "ZExpr.h"
#include "ZArg.h"
#include "ZGenericParam.h"
#include "ZFuncType.h"

class ZArg;

class ZFunc : public ZExpr {
public:	
	ZFunc(std::string* name, ZType* returnType, std::vector<ZArg*>& args, std::vector<ZGenericParam*>& typeParams, ZAst* body, bool isExtern = false) : _args(args) {
        _name = name;
        _returnType = returnType;
		_typeParams = typeParams;
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

	bool isGeneric() {
		return _typeParams.size() > 0;
	}

    string* getName() {
        return _name;
	}

    vector<ZArg*>& getArgs() {
        return _args;
	}

    ZType* getReturnType() {
        return _returnType;
	}

    ZAst* getBody() {
        return _body;
	}

	std::vector<ZGenericParam*> getTypeParams() {
		return _typeParams;
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

private:
    std::string* _name;
	ZType* _returnType;
    std::vector<ZArg*>& _args;
	std::vector<ZGenericParam*> _typeParams;
	ZAst* _body;
	bool _isExtern;
};

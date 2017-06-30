#pragma once

#include "ZAst.h"
#include "SymbolEntry.h"

class ZType;

class ZExpr : public ZAst {
public:
    //virtual llvm::Value* codegen(llvm::Function * func) = 0;

	ZType* getType() {
		return _type;
	}

	void setType(ZType* type) {
		_type = type;
	}

private:
	ZType* _type = Unknown;
};

#pragma once

#include "ZBasicTypes.h"
#include "ZAst.h"

class ZExpr : public ZAst {
public:
    //virtual llvm::Value* codegen(llvm::Function * func) = 0;

	BaseTypes getType()	{
		return _type;
	}

	void setType(BaseTypes type) {
		_type = type;
	}

private:
	BaseTypes _type = Unknown;
};

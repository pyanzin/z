#pragma once

#include "ZAst.h"
#include "SymbolEntry.h"

class ZType;

class ZExpr : public ZAst {
public:
	virtual ZType* getType() {
		return _type;
	}

	virtual void setType(ZType* type) {
		_type = type;
	}

private:
	ZType* _type = Unknown;
};

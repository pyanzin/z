#pragma once

#include "ZAst.h"
#include "SymbolEntry.h"
#include "SymbolRef.h"

class ZType;

class ZExpr : public ZAst {
public:
	virtual ZType* getType() {
		return _type;
	}

	virtual void setType(ZType* type) {
		_type = type;
	}

    void setRef(SymbolRef* ref) {
        _ref = ref;
	}

    SymbolRef* getRef() {
        return _ref;
    }
protected:
    SymbolRef* _ref = nullptr;
private:
    ZType* _type = Unknown;    
};

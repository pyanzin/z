#pragma once

#include "ZAst.h"
#include "SymbolEntry.h"
#include "SymbolRef.h"
#include "CompilerException.h"

class ZType;

class ZExpr : public ZAst {
public:
    virtual ZType* getType() {
        if (_ref) {
            auto resolved = _ref->resolveIfDelayed(_type);
            _type = resolved;
        }
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

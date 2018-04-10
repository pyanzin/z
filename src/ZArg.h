#pragma once

#include "ZAst.h"
#include "SymbolRef.h"

class ZVisitor;
class ZType;

class ZArg : public ZAst {
public:

    ZArg(ZType* type, std::string* name, SymbolRef* ref) {
        _type = type;
        _name = name;
        _ref = ref;
    }

    ZType* getType() {
        if (_ref)
            _type = _ref->resolveIfDelayed(_type);
        return _type;
    }

    void setType(ZType* type) {
        _type = type;
    }

    ZType* getRawType() {
        return _type;
    }

    std::string* getName() {
        return _name;
    }

    virtual void accept(ZVisitor* visitor);
private:
    ZType* _type;
    std::string* _name;
    SymbolRef* _ref;
};

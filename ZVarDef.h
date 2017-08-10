#pragma once
#include <string>
#include "ZBasicTypes.h"
#include "ZAst.h"
#include "ZExpr.h"
#include "SymbolRef.h"


class ZVarDef : public ZAst {
public:
	

	ZVarDef(std::string& name, SymbolRef& ref, ZType* type = nullptr, ZExpr* initExpr = nullptr) : _name(name), _ref(ref) {
        _type = type;
        _initExpr = initExpr;
    }

	virtual void accept(ZVisitor* visitor);

    std::string& getName() {
        return _name;
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "var " << _name << " : " << _type->toString() << "\n";

        if (_initExpr)
            _initExpr->dump(stream, depth + 1);
	}

	ZExpr* getInitExpr() {
		return _initExpr;
    }

	ZType* getVarType() {
		return _type;
    }

	void setVarType(ZType* type) {
		_type = type;
		_ref.getEntry()->setType(type);
    }

	SymbolRef& getRef() {
		return _ref;
    };
private:
    std::string& _name;
	SymbolRef& _ref;
    ZType* _type;
    ZExpr* _initExpr;
	

};

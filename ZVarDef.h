#pragma once
#include <string>
#include "ZBasicTypes.h"
#include "ZAst.h"
#include "ZExpr.h"


class ZVarDef : public ZAst {
public:
    ZVarDef(std::string& name, ZType* type, ZExpr* initExpr = nullptr) : _name(name) {
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

private:
    std::string& _name;
    ZType* _type;
    ZExpr* _initExpr;

};

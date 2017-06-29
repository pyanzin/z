#pragma once
#include "ZAst.h"
#include <string>
#include "ZBasicTypes.h"
#include "ZStringLit.h"

class ZVarDef : public ZAst {
public:
    ZVarDef(std::string& name, BaseTypes type) : _name(name) {
        _type = type;
    }

	virtual void accept(ZVisitor* visitor) {
		visitor->visit(this);
	}

    std::string& getName() {
        return _name;
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "var " << _name << " : " << toString(_type) << "\n";
	}

private:
    std::string& _name;
    BaseTypes _type;
};

#pragma once
#include <string>
#include "ZBasicTypes.h"
#include "ZAst.h"



class ZVarDef : public ZAst {
public:
    ZVarDef(std::string& name, ZType* type) : _name(name) {
        _type = type;
    }

	virtual void accept(ZVisitor* visitor);

    std::string& getName() {
        return _name;
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "var " << _name << " : " << _type->toString() << "\n";
	}

private:
    std::string& _name;
    ZType* _type;
};

#pragma once

#include "ZExpr.h"
#include "Utils.h"
#include "SymbolRef.h"

class ZId : public ZExpr {
public:
    ZId(std::string& name, SymbolRef* ref) : _name(name) {
        setRef(ref);
    }

	virtual void accept(ZVisitor* visitor) override {
		visitor->visit(this);
	}

	std::string& getName() {
		return _name;
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "id(" << _name << ") : "<< getType()->toString() << "\n";
    }

private:
    std::string& _name;
};
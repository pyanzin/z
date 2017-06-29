#pragma once
#include <ostream>
#include "ZVisitor.h"

class ZVisitor;

class ZAst {
public:
    virtual void accept(ZVisitor* visitor) {
		visitor->visit(this);
    };

	virtual void dump(std::ostream& stream, unsigned depth) {
		// dump in child only if it's really needed
	}

	void dumpTab(std::ostream& stream, unsigned depth) {
		unsigned d = 0;
		while (d++ < depth)
			stream << "    ";
	}
};

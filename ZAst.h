#pragma once
#include <ostream>

class ZVisitor;

class ZAst {
public:
    virtual void accept(ZVisitor* visitor);

	virtual void dump(std::ostream& stream, unsigned depth) {
		// dump in child only if it's really needed
	};
};

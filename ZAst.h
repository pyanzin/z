#pragma once
#include <ostream>
#include "SourceRange.h"

class ZVisitor;

class ZAst {
public:
	virtual void accept(ZVisitor* visitor);;

	virtual void dump(std::ostream& stream, unsigned depth) {
		// dump in child only if it's really needed
	}

	void dumpTab(std::ostream& stream, unsigned depth) {
		unsigned d = 0;
		while (d++ < depth)
			stream << "    ";
	}

    ZAst* withSourceRange(SourceRange* sr) {
        _sourceRange = sr;
		return this;
	}

	std::string getPosition() {
		return getSourceRange()->getPosition();
	}

    SourceRange* getSourceRange() {
        if (!_sourceRange)
            throw std::exception("Source range is not set before using");

        return _sourceRange;
	}

private:
    SourceRange* _sourceRange = nullptr;
};

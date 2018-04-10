#pragma once
#include <ostream>
#include "SourceRange.h"

class ZVisitor;

class ZAst {
public:
    virtual void accept(ZVisitor* visitor);

    void adopt(ZAst* child) {
        child->_parent = this;
    }

    virtual void replaceChild(ZAst* oldChild, ZAst* newChild) {
        throw exception("wrong call to replaceChild");
    }

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
        SourceRange* sr = getSourceRange();
        return sr ? sr->getPosition() : string("0:0");
    }

    SourceRange* getSourceRange() {
        return _sourceRange;
    }

    ZAst* getParent() {
        return _parent;
    }

private:
    SourceRange* _sourceRange = nullptr;
    ZAst* _parent = nullptr;
};

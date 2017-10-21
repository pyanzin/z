#pragma once
#include "ZAst.h"
#include "ZExpr.h"
#include "ZVisitor.h"

class ZSubscript : public ZExpr {
public:
    ZSubscript(ZExpr* target, ZExpr* index) {
        _target = target;
        _index = index;
    }

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    }

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);
        if (_target == oldChild) {
            _target = static_cast<ZExpr*>(newChild);
            return;
        }

        if (_index == oldChild) {
            _index = static_cast<ZExpr*>(newChild);
            return;
        }

        throw exception("wrong call to replaceChild in ZSubscript");
    }

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << "subscript\n";
        _target->dump(stream, depth + 1);

        dumpTab(stream, depth);
        stream << "index\n";
        _index->dump(stream, depth + 1);        
    }

    ZExpr* getTarget() {
        return _target;
    }

    ZExpr* getIndex() {
        return _index;
    }
private:
    ZExpr* _target;
    ZExpr* _index;
};

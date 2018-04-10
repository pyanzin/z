#pragma once
#include "ZAst.h"
#include "ZExpr.h"
#include "ZVisitor.h"

class ZReturn : public ZAst {
public:
    ZReturn(ZExpr* expr) {
        _expr = expr;
        if (_expr)
            adopt(expr);
    }

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    }

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);
        if (_expr == oldChild) {
            _expr = static_cast<ZExpr*>(newChild);
            return;
        }

        throw exception("wrong call to replaceChild in ZReturn");
    }

    ZExpr* getExpr() {
        return _expr;
    }

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << "return\n";
        if (_expr)
            _expr->dump(stream, depth + 1);
        else {
            dumpTab(stream, depth + 1);
            stream << "void\n";
        }
    }

private:
    ZExpr* _expr;

};

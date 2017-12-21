#pragma once
#include "ZExpr.h"
#include "Utils.h"
#include "ZVisitor.h"

class ZUnaryOp : public ZExpr {
public:
    ZUnaryOp(ZExpr* target, UnaryOps op) {
        adopt(target);
        _target = target;
        _op = op;
    }

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);
        if (_target == oldChild)
            _target = static_cast<ZExpr*>(newChild);

        else throw exception("wrong call to replaceChild in ZUnaryOp");
    }

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    };

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << toString(_op) << "\n";
        _target->dump(stream, depth + 1);
    }

    ZExpr* getTarget() {
        return _target;
    }

    UnaryOps getOp() {
        return _op;
    }

private:
    ZExpr* _target;
    UnaryOps _op;
};

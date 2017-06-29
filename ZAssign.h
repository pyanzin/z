#pragma once
#include "ZExpr.h"
#include "ZStringLit.h"

class ZAssign : public ZExpr {
public:
    ZAssign(ZExpr* left, ZExpr* right) {
        _left = left;
        _right = right;
    }

	void accept(ZVisitor* visitor) override {
		visitor->visit(this);
    }

    ZExpr* _left;
    ZExpr* _right;

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << "assign : " << toString(getType()) << "\n";
        _left->dump(stream, depth + 1);
        _right->dump(stream, depth + 1);
    }
};

#pragma once
#include "ZExpr.h"
#include "ZBasicTypes.h"

class ZAssign : public ZExpr {
public:
	ZAssign(ZExpr* left, ZExpr* right) {
		_left = left;
		_right = right;
	}

	void accept(ZVisitor* visitor) override;

	ZExpr* getLeft() { return _left; }
	ZExpr* getRight() { return _right; }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "assign : " << getType()->toString() << "\n";
		_left->dump(stream, depth + 1);
		_right->dump(stream, depth + 1);
	}
private:
	ZExpr* _left;
	ZExpr* _right;
};

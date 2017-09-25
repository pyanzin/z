#ifndef ZBINOP
#define ZBINOP
#include "Utils.h"
#include "ZExpr.h"
#include "ZVisitor.h"

class ZBinOp : public ZExpr {
public:
	ZBinOp(ZExpr* a, ZExpr* b, BinOps op) {
		_left = a;
		_right = b;
		_op = op;

		adopt(a);
		adopt(b);
	}

	void replaceChild(ZAst* oldChild, ZAst* newChild) override {
		adopt(newChild);
		if (_left == oldChild)
			_left = static_cast<ZExpr*>(newChild);
		else if (_right == oldChild)
			_right = static_cast<ZExpr*>(newChild);
		else throw exception("wrong call to replaceChild in ZBinOp");
	}

	void accept(ZVisitor* visitor) override;

	ZExpr* getLeft() { return _left; }
	ZExpr* getRight() { return _right; }
	BinOps getOp() { return _op; }

	void dump(std::ostream& stream, unsigned depth) override;

private:
	ZExpr* _left;
	ZExpr* _right;
	BinOps _op;
};

#endif

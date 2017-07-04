#ifndef ZBINOP
#define ZBINOP
#include "Utils.h"
#include "ZExpr.h"
#include "ZVisitor.h"

class ZBinOp : public ZExpr {
public:
	ZBinOp(ZExpr* a, ZExpr* b, BinOps op) {
		_a = a;
		_b = b;
		_op = op;
	}

	void accept(ZVisitor* visitor) override;



	ZExpr* getLeft() { return _a; }
	ZExpr* getRight() { return _b; }
	BinOps getOp() { return _op; }

	void dump(std::ostream& stream, unsigned depth) override;

private:
	ZExpr* _a;
	ZExpr* _b;
	BinOps _op;
};

#endif

#ifndef ZBINOP
#define ZBINOP
#include "Utils.h"
#include "ZExpr.h"

class ZBinOp : public ZExpr {
public:
	ZBinOp(ZExpr* a, ZExpr* b, BinOps op) {
		_a = a;
		_b = b;
		_op = op;
	}

	Value* codegen(llvm::Function * func) override {
		switch (_op)
		{
		case Sum:
			return builder.CreateAdd(_a->codegen(func), _b->codegen(func));
		case Sub:
			return builder.CreateSub(_a->codegen(func), _b->codegen(func));
		case Mul:
			return builder.CreateMul(_a->codegen(func), _b->codegen(func));
		case Div:
			return builder.CreateSDiv(_a->codegen(func), _b->codegen(func));
		default:
			return nullptr;
		}
	}

	ZExpr* getLeft() { return _a; }
	ZExpr* getRight() { return _b; }
	BinOps getOp() { return _op; }

	virtual void accept(ZVisitor* visitor) {
		_a->accept(visitor);
		_b->accept(visitor);
		visitor->visit(this);
	}

private:
	ZExpr* _a;
	ZExpr* _b;
	BinOps _op;
};

#endif
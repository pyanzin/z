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

	void accept(ZVisitor* visitor) override {
		visitor->visit(this);
	}

	//Value* codegen(llvm::Function * func) override {
	//	switch (_op)
	//	{
	//	case Sum:
	//		return builder.CreateAdd(_a->codegen(func), _b->codegen(func));
	//	case Sub:
	//		return builder.CreateSub(_a->codegen(func), _b->codegen(func));
	//	case Mul:
	//		return builder.CreateMul(_a->codegen(func), _b->codegen(func));
	//	case Div:
	//		return builder.CreateSDiv(_a->codegen(func), _b->codegen(func));
	//	default:
	//		return nullptr;
	//	}
	//}

	ZExpr* getLeft() { return _a; }
	ZExpr* getRight() { return _b; }
	BinOps getOp() { return _op; }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << toString(_op) << " : " << toString(getType()) << "\n";
		_a->dump(stream, depth + 1);
		_b->dump(stream, depth + 1);
	}

private:
	ZExpr* _a;
	ZExpr* _b;
	BinOps _op;
};

#endif
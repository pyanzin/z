#pragma once
#include "ZAst.h"
#include "ZExpr.h"
#include "ZVisitor.h"

class ZReturn : public ZAst {
public:
	ZReturn(ZExpr* expr) {
		_expr = expr;
	}

	void accept(ZVisitor* visitor) override {
		visitor->visit(this);
	}

	ZExpr* getExpr() {
		return _expr;
	}

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "return\n";
		_expr->dump(stream, depth + 1);
	}
private:
	ZExpr* _expr;

};

#pragma once
#include "ZVisitor.h"
#include "ZModule.h"
#include "ZBlock.h"
#include "ZBinOp.h"
#include "ZAssign.h"
#include "ZCall.h"

class TypingPass : public ZVisitor {
public:
	void visit(ZModule* zmodule) override {
		for (ZFunc* zf : zmodule->getFunctions())
			zf->accept(this);		
	}

	void visit(ZFunc* zfunc) override {
		zfunc->_body->accept(this);
	}

	void visit(ZBlock* zblock) override {
		for (ZAst* stmt : zblock->_expressions)
			stmt->accept(this);
	}

	void visit(ZAssign* zassign) override {
		zassign->_left->accept(this);
		zassign->_right->accept(this);

		zassign->setType(zassign->_right->getType());
	}

	void visit(ZCall* zcall) override {
		for (ZExpr* arg : zcall->getArgs())
			arg->accept(this);
	}

	void visit(ZBinOp* zbinop) override {
		ZExpr* left = zbinop->getLeft();
		ZExpr* right = zbinop->getRight();

		left->accept(this);
		right->accept(this);

		if (zbinop->getOp() == Sum && left->getType() == String) {
			zbinop->setType(String);
			return;
		}

		if (left->getType() == Int && right->getType() == Int) {
			zbinop->setType(Int);
			return;
		}

		// todo: doubles, ints and combinations

		error("Unable to apply operation " + toString(zbinop->getOp()) + " for "
			+ toString(left->getType()) + " and " + toString(right->getType()));
	}

	void visit(ZId* zid) override {

	}
};

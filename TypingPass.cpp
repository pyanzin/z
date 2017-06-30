#include "TypingPass.h"
#include "ZExpr.h"
#include "Utils.h"
#include "ZStringLit.h"
#include "ZModule.h"
#include "ZAssign.h"
#include "ZBlock.h"
#include "ZCall.h"
#include "ZBinOp.h"

void TypingPass::visit(ZModule* zmodule) {
	for (ZFunc* zf : zmodule->getFunctions())
		zf->accept(this);
}

void TypingPass::visit(ZFunc* zfunc) {
	zfunc->_body->accept(this);
}

void TypingPass::visit(ZBlock* zblock) {
	for (ZAst* stmt : zblock->_expressions)
		stmt->accept(this);
}

void TypingPass::visit(ZAssign* zassign) {
	zassign->_left->accept(this);
	zassign->_right->accept(this);

	zassign->setType(zassign->_right->getType());
}

void TypingPass::visit(ZCall* zcall) {
	for (ZExpr* arg : zcall->getArgs())
		arg->accept(this);
}

void TypingPass::visit(ZBinOp* zbinop) {
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
		+ left->getType()->toString() + " and " + right->getType()->toString());
}

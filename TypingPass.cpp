#include "TypingPass.h"
#include "ZExpr.h"
#include "Utils.h"
#include "ZStringLit.h"
#include "ZModule.h"
#include "ZAssign.h"
#include "ZBlock.h"
#include "ZCall.h"
#include "ZBinOp.h"
#include "ZId.h"
#include "ZArg.h"
#include "ZReturn.h"

void TypingPass::visit(ZModule* zmodule) {
	for (ZFunc* zf : zmodule->getFunctions())
		zf->accept(this);
}

void TypingPass::visit(ZFunc* zfunc) {
	for (auto arg : zfunc->_args)
		arg->accept(this);
	zfunc->_body->accept(this);
}

void TypingPass::visit(ZBlock* zblock) {
	for (ZAst* stmt : zblock->getStatements())
		stmt->accept(this);
}

void TypingPass::visit(ZAssign* zassign) {
	zassign->_left->accept(this);
	zassign->_right->accept(this);

	zassign->setType(zassign->_right->getType());
}

void TypingPass::visit(ZCall* zcall) {
	zcall->callee->accept(this);
	for (ZExpr* arg : zcall->getArgs())
		arg->accept(this);

	ZType* retType = static_cast<ZFuncType*>(zcall->callee->getType())->getRetType();
	zcall->setType(retType);
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

	if (zbinop->getOp() >= BinOps::Equal && zbinop->getOp() <= BinOps::MoreOrEqual) {
		zbinop->setType(Boolean);
		return;
	}

	// todo: doubles, ints and combinations

	error("Unable to apply operation " + toString(zbinop->getOp()) + " for "
		+ left->getType()->toString() + " and " + right->getType()->toString());
}

void TypingPass::visit(ZId* zid) {
    SymbolEntry* definition = zid->getRef().findDefinedBefore(zid->getName());
    if (definition)
        zid->setType(definition->getType());
}

void TypingPass::visit(ZReturn* zreturn) {
	zreturn->getExpr()->accept(this);
}

void TypingPass::visit(ZVarDef* zvardef) {
	zvardef->getInitExpr()->accept(this);
}

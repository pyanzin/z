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
#include "ZIf.h"
#include "ZWhile.h"
#include "ZVarDef.h"
#include "ZSubscript.h"
#include "ZArrayType.h"

void TypingPass::visit(ZModule* zmodule) {
	for (ZFunc* zf : zmodule->getFunctions())
		visit(zf);
}

void TypingPass::visit(ZFunc* zfunc) {
	_func = zfunc;
	for (auto arg : zfunc->_args)
		arg->accept(this);

	if (!zfunc->isExtern())
		zfunc->_body->accept(this);	
}

void TypingPass::visit(ZBlock* zblock) {
	for (ZAst* stmt : zblock->getStatements())
		stmt->accept(this);
}

void TypingPass::visit(ZAssign* zassign) {
	zassign->getLeft()->accept(this);
	zassign->getRight()->accept(this);

    ZExpr* assignee = dynamic_cast<ZId*>(zassign->getLeft());

    if (!assignee)
        assignee = dynamic_cast<ZSubscript*>(zassign->getLeft());

    if (!assignee)
        error("Left part of the assignment expression is not suitable for assignment", zassign->getPosition());

    if (assignee->getType() != zassign->getRight()->getType())
        error("Type of the left hand expression doesn't match the type of right hand expression", assignee->getPosition());

	zassign->setType(zassign->getRight()->getType());
}

void TypingPass::visit(ZCall* zcall) {
	zcall->callee->accept(this);

    int callerArgsCount = zcall->getArgs().size();

    if (zcall->callee->getType()->getName() == "Array") {
        if (callerArgsCount != 1)
            error("Array subsciprt operator requires 1 argument");

        ZSubscript* zsubscript = new ZSubscript(zcall->callee, zcall->getArgs()[0]);
        zcall->getParent()->replaceChild(zcall, zsubscript);
        visit(zsubscript);
        return;
    }

	for (ZExpr* arg : zcall->getArgs())
		arg->accept(this);

	ZFuncType* calleeType = dynamic_cast<ZFuncType*>(zcall->callee->getType());

	if (!calleeType)
		error("Type of calee is not callable");

	int calleeArgsCount = calleeType->getArgTypes().size();
	

	if (calleeArgsCount != callerArgsCount)
		error("Callee requires " + std::to_string(calleeArgsCount) + ", but caller passes " + std::to_string(callerArgsCount));
	int i = 0;
	for (ZType* calleeArgType : calleeType->getArgTypes()) {
		ZType* callerArgType = zcall->getArgs()[i++]->getType();
		if (!calleeArgType->isEqual(*callerArgType))
			error("Callee expects argument of type " + calleeArgType->toString() 
				+ ", but received " + callerArgType->toString() 
				+ " at position " + std::to_string(i));
		
	}

	ZType* retType = static_cast<ZFuncType*>(zcall->callee->getType())->getRetType();
	zcall->setType(retType);
}

void TypingPass::visit(ZSubscript* zsubscript) {
    ZExpr* target = zsubscript->getTarget();
    target->accept(this);
    ZArrayType* targetType = dynamic_cast<ZArrayType*>(target->getType());

    if (!targetType)
        error("Subscript operator is only applicable for expressions of array type");

    ZExpr* index = zsubscript->getIndex();
    index->accept(this);
    if (index->getType() != Int && index->getType() != Char)
        error("Subscript index must have integer type");

    zsubscript->setType(targetType->getElementType());
}


void TypingPass::visit(ZBinOp* zbinop) {
	zbinop->getLeft()->accept(this);
	zbinop->getRight()->accept(this);

	if (zbinop->getOp() == Sum && zbinop->getLeft()->getType() == String && zbinop->getRight()->getType() == String) {
        auto parent = zbinop->getParent();
        auto args = new std::vector<ZExpr*>();
		args->push_back(zbinop->getLeft());
		args->push_back(zbinop->getRight());
        auto zid = new ZId(*(new std::string("concat")), zbinop->getRef());
        zid->withSourceRange(zbinop->getSourceRange());
        auto concat = new ZCall(zid, *args);
        concat->withSourceRange(zbinop->getSourceRange());
        parent->replaceChild(zbinop, concat);
        concat->accept(this);
		return;
	}

	if (zbinop->getOp() >= BinOps::Equal && zbinop->getOp() <= BinOps::MoreOrEqual) {
		zbinop->setType(Boolean);
		return;
	}

	if (zbinop->getLeft()->getType() == Int && zbinop->getRight()->getType() == Int) {
		zbinop->setType(Int);
		return;
	}

	// todo: doubles, ints and combinations

	error("Unable to apply operation " + toString(zbinop->getOp()) + " for "
		+ zbinop->getLeft()->getType()->toString() + " and " + zbinop->getRight()->getType()->toString(), zbinop->getPosition());
}

void TypingPass::visit(ZId* zid) {
    SymbolEntry* definition = zid->getRef()->findSymbolDef(zid->getName());
    if (!definition)
        error("Symbol '" + zid->getName() + "' is not defined before using", zid->getPosition());
        
    zid->setType(definition->getType());
}

void TypingPass::visit(ZReturn* zreturn) {
    if (zreturn->getExpr())      
        zreturn->getExpr()->accept(this);
	
    ZType* retType = zreturn->getExpr() ? zreturn->getExpr()->getType() : Void;

	if (_func->_returnType != retType)
		error("Type of return statement doesn't match function return type", zreturn->getPosition());
}

void TypingPass::visit(ZIf* zif) {
	zif->getCondition()->accept(this);
	
	if (zif->getCondition()->getType() != Boolean)
		error("Condition of if statement must be of Boolean type", zif->getPosition());

	zif->getBody()->accept(this);
	if (zif->getElseBody())
		zif->getElseBody()->accept(this);
}

void TypingPass::visit(ZWhile* zwhile) {
	zwhile->getCondition()->accept(this);

	if (zwhile->getCondition()->getType() != Boolean)
		error("Condition of while statement must be of Boolean type", zwhile->getPosition());	

	zwhile->getBody()->accept(this);
}

void TypingPass::visit(ZVarDef* zvardef) {
	SymbolEntry* alreadyDefined = zvardef->getRef().findSymbolDef(zvardef->getName(), true);

	if (alreadyDefined)
		error("Variable with name '" + alreadyDefined->getName() + "' already defined in this scope", zvardef->getPosition());	

	auto initExpr = zvardef->getInitExpr();

	initExpr->accept(this);

	if (!zvardef->getVarType() || zvardef->getVarType() == Unknown)
		zvardef->setVarType(zvardef->getInitExpr()->getType());
	else if (zvardef->getVarType() != initExpr->getType())
		error("Type of variable '" + zvardef->getName() + "' doesn't match the type of init expression", zvardef->getPosition());
			
}

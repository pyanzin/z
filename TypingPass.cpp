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
#include "ZGenericParam.h"
#include "ZLambda.h"
#include "ZCast.h"
#include "ZFor.h"
#include "ZSelector.h"
#include "ZStructType.h"

class ZFuncCast;

void TypingPass::visit(ZModule* zmodule) {
    _module = zmodule;
	for (ZFunc* zf : zmodule->getFunctions())
		visit(zf);
}

void TypingPass::visit(ZFunc* zfunc) {
	_func = zfunc;
	for (auto arg : zfunc->getArgs())
		arg->accept(this);

	if (!zfunc->isExtern())
		zfunc->getBody()->accept(this);	
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
		assignee = dynamic_cast<ZSelector*>(zassign->getLeft());

    if (!assignee)
        error("Left part of the assignment expression is not suitable for assignment", zassign->getPosition());

    if (!dynamic_cast<ZSubscript*>(assignee) && !assignee->getType()->isEqual(*zassign->getRight()->getType()))
        error("Type of the left hand expression doesn't match the type of right hand expression", assignee->getPosition());

	zassign->setType(zassign->getRight()->getType());
}

void TypingPass::visit(ZLambda* zlambda) {
    for (auto arg : *zlambda->getArgs())
        arg->accept(this);

    zlambda->getBody()->accept(this);

    ZExpr* exprBody = dynamic_cast<ZExpr*>(zlambda->getBody());

    auto bodyType = exprBody ? exprBody->getType() : Unknown;

    if (zlambda->getReturnType()->isEqual(*Unknown))
        zlambda->setRetType(bodyType);
    else if (exprBody && !zlambda->getReturnType()->isEqual(*bodyType))
        error("Return type doesn't match the type of lambda body");
}

void TypingPass::visit(ZCall* zcall) {
	zcall->callee->accept(this);

    int callerArgsCount = zcall->getArgs().size();

	if (dynamic_cast<ZArrayType*>(zcall->callee->getType()) || zcall->callee->getType()->isEqual(*String)) {
        if (callerArgsCount != 1)
            error("Array subsciprt operator requires 1 argument");

        ZSubscript* zsubscript = new ZSubscript(zcall->callee, zcall->getArgs()[0]);
        zcall->getParent()->replaceChild(zcall, zsubscript);
        visit(zsubscript);
        return;
    }

	ZFuncType* calleeType = dynamic_cast<ZFuncType*>(zcall->callee->getType());

	if (!calleeType)
		error("Type of calee is not callable");

	int calleeArgsCount = calleeType->getParamTypes().size();

	if (calleeArgsCount != callerArgsCount)
		error("Callee requires " + std::to_string(calleeArgsCount) + " arguments, but caller passes " + std::to_string(callerArgsCount));
	
	bool calleeIsGeneric = calleeType->hasGenericDefs();

    if (calleeIsGeneric) {
        bool typeArgsPassed = zcall->getTypeArgs() && zcall->getTypeArgs()->size() > 0;
        for (int i = 0; i < calleeType->getGenericDefs()->size(); ++i) {
            auto param = (*calleeType->getGenericDefs())[i];
            auto arg = typeArgsPassed ? (*zcall->getTypeArgs())[i] : Unknown;
            zcall->getRef()->addResolution(param, arg);
        }

        juxtapose(calleeType, zcall);
    } else {
        for (ZExpr* arg : zcall->getArgs())
            arg->accept(this);

		int i = 0;
		for (ZType* calleeArgType : calleeType->getParamTypes()) {
			ZExpr* arg = zcall->getArgs()[i++];
			ZType* callerArgType = arg->getType();
			if (calleeArgType->isEqual(*callerArgType))
				continue;
			if (callerArgType->canBeCastedTo(calleeArgType)) {
				ZCast* cast = new ZCast(arg, calleeArgType);
				zcall->replaceChild(arg, cast);
				continue;
			}
			error("Callee expects argument of type " + calleeArgType->toString()
				+ ", but received " + callerArgType->toString()
				+ " at position " + std::to_string(i));
		}       
	}

    SymbolRef* ref = zcall->getRef();

    ZType* retType = calleeType->getRetType();
    zcall->setType(ref->resolve(retType));
}

void TypingPass::visit(ZSubscript* zsubscript) {
    ZExpr* target = zsubscript->getTarget();
    target->accept(this);
    ZArrayType* arrayType = dynamic_cast<ZArrayType*>(target->getType());

	if (arrayType) 
		zsubscript->setType(arrayType->getElementType());
	else if (target->getType()->isEqual(*String))
		zsubscript->setType(Char);
	else
        error("Subscript operator is only applicable for expressions of array or string type");

    ZExpr* index = zsubscript->getIndex();
    index->accept(this);
    if (index->getType() != Int && index->getType() != Char)
        error("Subscript index must have integer type");
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
        auto concat = new ZCall(zid, *args, new std::vector<ZType*>, zbinop->getRef());
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

void TypingPass::visit(ZSelector* zselector) {
	zselector->getTarget()->accept(this);

	ZStructType* structType = dynamic_cast<ZStructType*>(zselector->getTarget()->getType());

	if (!structType)
		error("Unable to apply selector for non-structural type", zselector->getSourceRange()->getPosition());

	ZArg* member = structType->getMember(*zselector->getMember());
	zselector->setType(member->getType());
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

	if (!_func->getReturnType()->isEqual(*retType))
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

void TypingPass::visit(ZFor* zfor) {
	zfor->getPre()->accept(this);

	zfor->getCond()->accept(this);

	if (zfor->getCond()->getType() != Boolean)
		error("Condition of for statement must be of Boolean type", zfor->getPosition());

	if (zfor->getPost())
		zfor->getPost()->accept(this);

	zfor->getBody()->accept(this);
}

void TypingPass::visit(ZVarDef* zvardef) {
	SymbolEntry* alreadyDefined = zvardef->getRef().findSymbolDef(zvardef->getName(), true);

	if (alreadyDefined)
		error("Variable with name '" + alreadyDefined->getName() + "' already defined in this scope", zvardef->getPosition());	

	auto initExpr = zvardef->getInitExpr();

	initExpr->accept(this);

	if (!zvardef->getVarType() || zvardef->getVarType()->isEqual(*Unknown))
		zvardef->setVarType(zvardef->getInitExpr()->getType());
	else if (!zvardef->getVarType()->isEqual(*initExpr->getType()))
		error("Type of variable '" + zvardef->getName() + "' doesn't match the type of init expression", zvardef->getPosition());
			
}

void TypingPass::visit(ZCast* zcast) {
	zcast->getExpr()->accept(this);
}

void TypingPass::visit(ZFuncCast* zfunccast) {
	// TODO: drag Symbol Ref here

	ZAst* parent = zfunccast->getParent();

	ZFuncType* targetType = zfunccast->getTargetType();
	std::vector<ZArg*>* targetParams = new std::vector<ZArg*>();
	std::vector<ZExpr*>* originArgs = new std::vector<ZExpr*>();
	int i = 0;
	for (ZType* paramType : targetType->getParamTypes()) {
		std::string* paramName = new string("__p" + i++);
		targetParams->push_back(new ZArg(paramType, paramName));
		originArgs->push_back(new ZCast(new ZId(*paramName, nullptr), paramType));
	}

	ZCall* callToOriginal = new ZCall(zfunccast->getExpr(), *originArgs, new std::vector<ZType*>, nullptr);

	ZExpr* body = new ZCast(callToOriginal, zfunccast->getTargetType());

	ZLambda* wrapperLambda = new ZLambda(targetParams, targetType->getRetType(), body, nullptr);

	parent->replaceChild(zfunccast, wrapperLambda);

	wrapperLambda->accept(this);

}

void TypingPass::juxtapose(ZType* calleeType, ZCall* call) {
	auto funcType = dynamic_cast<ZFuncType*>(calleeType);

	SymbolRef* ref = call->getRef();

    bool isTypeArgsDefined = call->getTypeArgs()->size();
	int i = 0;
	for (ZGenericParam* param : *funcType->getGenericDefs())
        ref->addResolution(param, isTypeArgsDefined ? (*call->getTypeArgs())[i++] : Unknown);

    for (int i = 0; i < call->getArgs().size(); ++i) {
        ZType* paramType = funcType->getParamTypes()[i];
        ZExpr* argExpr = call->getArgs()[i];
        juxtapose(paramType, argExpr, ref);
    }

	call->setType(juxtapose(funcType->getRetType(), call->getType(), ref));
}

void TypingPass::juxtapose(ZType* paramType, ZExpr* expr, SymbolRef* ref) {    
    ZType* type;
    auto lambda = dynamic_cast<ZLambda*>(expr);
    if (lambda) {
        type = juxtapose(paramType, expr->getType(), ref);
        expr->setType(type);
        expr->accept(this);
        type = juxtapose(paramType, expr->getType(), ref);

    } else {
        expr->accept(this);
        type = juxtapose(paramType, expr->getType(), ref);
    }
}

ZType* TypingPass::juxtapose(ZType* paramType, ZType* argType, SymbolRef* ref) {
    ZGenericParam* gen = dynamic_cast<ZGenericParam*>(paramType);
    if (gen) {
        ZType* resolved = ref->resolve(paramType);
        if (!resolved->isEqual(*Unknown)) {
            if (argType->isEqual(*Unknown))
                return resolved;
            if (resolved->isEqual(*argType))
                return argType;
            error("Type mismatch: expected " + resolved->toString() + ", but passed " + argType->toString()); 
        } else {
            if (argType->isEqual(*Unknown))
                return argType;
            ref->addResolution(gen, argType);
            return argType;
        }
    }

    if (argType->isEqual(*Unknown))
        argType = paramType->copyStem();

    for (int i = 0; i < paramType->getTypeParams()->size(); ++i)
        argType->setTypeParam(i, juxtapose((*paramType->getTypeParams())[i], (*argType->getTypeParams())[i], ref));
        
    return argType;
}


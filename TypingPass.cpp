#include "TypingPass.h"
#include "ZExpr.h"
#include "Utils.h"
#include "ZStringLit.h"
#include "ZModule.h"
#include "ZAssign.h"
#include "ZBlock.h"
#include "ZCall.h"
#include "ZBinOp.h"
#include "ZUnaryOp.h"
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
#include "Juxtaposer.h"
#include "ZNumberType.h"

class ZFuncCast;

void TypingPass::visit(ZModule* zmodule) {
    _module = zmodule;
    for (ZFunc* zf : zmodule->getFunctions()) {
        try {
            visit(zf);
        } catch (RecoveryException) {
            
        }
    }
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
        error("Left part of the assignment expression is not suitable for assignment", zassign->getSourceRange());

    if (!dynamic_cast<ZSubscript*>(assignee) && !assignee->getType()->isEqual(*zassign->getRight()->getType()))
        error("Type of the left hand expression doesn't match the type of right hand expression", assignee->getSourceRange());

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
        error("Return type doesn't match the type of lambda body", zlambda->getSourceRange());
}

void TypingPass::visit(ZCall* zcall) {
	zcall->callee->accept(this);

    int callerArgsCount = zcall->getArgs().size();

	if (dynamic_cast<ZArrayType*>(zcall->callee->getType()) || zcall->callee->getType()->isEqual(*String)) {
        if (callerArgsCount != 1)
            error("Array subsciprt operator requires 1 argument", zcall->getSourceRange());

        ZSubscript* zsubscript = new ZSubscript(zcall->callee, zcall->getArgs()[0]);
        zcall->getParent()->replaceChild(zcall, zsubscript);
        visit(zsubscript);
        return;
    }

	ZFuncType* calleeType = dynamic_cast<ZFuncType*>(zcall->callee->getType());

	if (!calleeType)
		error("Type of calee is not callable", zcall->getSourceRange());

	int calleeArgsCount = calleeType->getParamTypes().size();

	if (calleeArgsCount != callerArgsCount)
		error("Callee requires " + std::to_string(calleeArgsCount) + " arguments, but caller passes " + std::to_string(callerArgsCount), 
            zcall->getSourceRange());
	
	bool calleeIsGeneric = calleeType->hasGenericDefs();

    if (calleeIsGeneric) {
        bool typeArgsPassed = zcall->getTypeArgs() && zcall->getTypeArgs()->size() > 0;
        for (int i = 0; i < calleeType->getGenericDefs()->size(); ++i) {
            auto param = (*calleeType->getGenericDefs())[i];
            auto arg = typeArgsPassed ? (*zcall->getTypeArgs())[i] : Unknown;
            zcall->getRef()->addResolution(param, arg);
        }

		auto juxtaposer = Juxtaposer(this);

        juxtaposer.juxtapose(calleeType, zcall);
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
				ZCast* cast = new ZCast(arg, calleeArgType, zcall->getRef());
				zcall->replaceChild(arg, cast);
                cast->accept(this);
				continue;
			}
			error("Callee expects argument of type " + calleeArgType->toString()
				+ ", but received " + callerArgType->toString()
				+ " at position " + std::to_string(i), zcall->getSourceRange());
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
        error("Subscript operator is only applicable for expressions of array or string type", zsubscript->getSourceRange());

    ZExpr* index = zsubscript->getIndex();
    index->accept(this);
    if (index->getType() != Int && index->getType() != Char)
        error("Subscript index must have integer type", zsubscript->getSourceRange());
}

void TypingPass::visit(ZBinOp* zbinop) {
	zbinop->getLeft()->accept(this);
	zbinop->getRight()->accept(this);

    bool isConcat = zbinop->getOp() == BinOps::Sum 
        && (zbinop->getLeft()->getType()->isEqual(*String) || zbinop->getRight()->getType()->isEqual(*String));
	if (isConcat) {
	    replaceWithConcat(zbinop);
	    return;        
	}

	if (zbinop->getOp() >= BinOps::Equal) {
		zbinop->setType(Boolean);
		return;
	}

	if (zbinop->getLeft()->getType() == Int && zbinop->getRight()->getType() == Int) {
		zbinop->setType(Int);
		return;
	}

	// todo: doubles, ints and combinations

	error("Unable to apply operation " + toString(zbinop->getOp()) + " for "
		+ zbinop->getLeft()->getType()->toString() + " and " + zbinop->getRight()->getType()->toString(), zbinop->getSourceRange());
}

void TypingPass::replaceWithConcat(ZBinOp* zbinop) {
    auto parent = zbinop->getParent();
    std::vector<ZExpr*>* args = new std::vector<ZExpr*>();
    args->push_back(makeToString(zbinop->getLeft(), zbinop->getRef()));
    args->push_back(makeToString(zbinop->getRight(), zbinop->getRef()));
    ZExpr* concat = makeCall("concat", args, zbinop->getRef());
    parent->replaceChild(zbinop, concat);
    concat->accept(this);
}

ZExpr* TypingPass::makeToString(ZExpr* expr, SymbolRef* ref) {
    ZType* type = expr->getType();

    if (type->isEqual(*String))
        return expr;

    std::vector<ZExpr*>* args = new std::vector<ZExpr*>();
    args->push_back(expr);

    std::string funcName;
    if (type->isEqual(*Int))
        funcName = "int2string";
    else if (type->isEqual(*Char))
        funcName = "char2string";
    else if (type->isEqual(*Double))
        funcName = "double2string";
    else if (type->isEqual(*Boolean))
        funcName = "boolean2string";
    else
        error("Unable to implicitly convert value to string", expr->getSourceRange());

    return makeCall(funcName, args, ref);   
}

ZExpr* TypingPass::makeCall(std::string funcName, std::vector<ZExpr*>* args, SymbolRef* ref) {
    auto zid = new ZId(*(new std::string(funcName)), ref);
    return new ZCall(zid, *args, new std::vector<ZType*>, ref);
}

void TypingPass::visit(ZUnaryOp* zunaryop) {
	zunaryop->getTarget()->accept(this);

	UnaryOps op = zunaryop->getOp();
	ZType* targetType = zunaryop->getTarget()->getType();

	if (op == Negation) {
		if (!targetType->isEqual(*Boolean))
			error("Negation operator is only applicable for expression of Boolean type", zunaryop->getSourceRange());
		zunaryop->setType(Boolean);
	} else {
		if (!dynamic_cast<ZNumberType*>(targetType))
			error("Operator " + toString(op) + " is only applicable to numeric types", zunaryop->getSourceRange());
		zunaryop->setType(targetType);
	}
}

void TypingPass::visit(ZSelector* zselector) {
	zselector->getTarget()->accept(this);

	ZStructType* structType = dynamic_cast<ZStructType*>(zselector->getTarget()->getType());

	if (!structType)
		error("Unable to apply selector for non-structural type", zselector->getSourceRange());

	ZArg* member = structType->getMember(*zselector->getMember());
	zselector->setType(member->getType());
}

void TypingPass::visit(ZId* zid) {
    SymbolEntry* definition = zid->getRef()->findSymbolDef(zid->getName());
    if (!definition)
        error("Symbol '" + zid->getName() + "' is not defined before using", zid->getSourceRange());
        
    zid->setType(definition->getType());
}

void TypingPass::visit(ZReturn* zreturn) {
    if (zreturn->getExpr())      
        zreturn->getExpr()->accept(this);
	
    ZType* retType = zreturn->getExpr() ? zreturn->getExpr()->getType() : Void;

	if (!_func->getReturnType()->isEqual(*retType))
		error("Type of return statement doesn't match function return type", zreturn->getSourceRange());
}

void TypingPass::visit(ZIf* zif) {
	zif->getCondition()->accept(this);
	
	if (zif->getCondition()->getType() != Boolean)
		error("Condition of if statement must be of Boolean type", zif->getSourceRange());

	zif->getBody()->accept(this);
	if (zif->getElseBody())
		zif->getElseBody()->accept(this);
}

void TypingPass::visit(ZWhile* zwhile) {
	zwhile->getCondition()->accept(this);

	if (zwhile->getCondition()->getType() != Boolean)
		error("Condition of while statement must be of Boolean type", zwhile->getSourceRange());	

	zwhile->getBody()->accept(this);
}

void TypingPass::visit(ZFor* zfor) {
	zfor->getPre()->accept(this);

	zfor->getCond()->accept(this);

	if (zfor->getCond()->getType() != Boolean)
		error("Condition of for statement must be of Boolean type", zfor->getSourceRange());

	if (zfor->getPost())
		zfor->getPost()->accept(this);

	zfor->getBody()->accept(this);
}

void TypingPass::visit(ZVarDef* zvardef) {
	SymbolEntry* alreadyDefined = zvardef->getRef().findSymbolDef(zvardef->getName(), true);

	if (alreadyDefined)
		error("Variable with name '" + alreadyDefined->getName() + "' already defined in this scope", zvardef->getSourceRange());	

	auto initExpr = zvardef->getInitExpr();

	initExpr->accept(this);

	if (!zvardef->getVarType() || zvardef->getVarType()->isEqual(*Unknown))
		zvardef->setVarType(zvardef->getInitExpr()->getType());
	else if (!zvardef->getVarType()->isEqual(*initExpr->getType()))
		error("Type of variable '" + zvardef->getName() + "' doesn't match the type of init expression", zvardef->getSourceRange());
			
}

void TypingPass::visit(ZCast* zcast) {
	zcast->getExpr()->accept(this);
    auto expr = zcast->getExpr();
    ZFuncType* funcTargetType = dynamic_cast<ZFuncType*>(zcast->getTargetType());
    if (dynamic_cast<ZFuncType*>(expr->getType()) && funcTargetType) {
        auto parent = zcast->getParent();
        auto zfunccast = new ZFuncCast(expr, funcTargetType, zcast->getRef());
        parent->replaceChild(zcast, zfunccast);
        zfunccast->accept(this);
    }
}

void TypingPass::visit(ZFuncCast* zfunccast) {
	SymbolRef* initRef = zfunccast->getRef();
	SymbolScope* newScope = initRef->getStorage()->makeChild();

	ZAst* parent = zfunccast->getParent();

	ZFuncType* targetType = zfunccast->getTargetType();
	std::vector<ZArg*>* targetParams = new std::vector<ZArg*>();
	std::vector<ZExpr*>* originArgs = new std::vector<ZExpr*>();
	int i = 0;
	for (ZType* paramType : targetType->getParamTypes()) {
		std::string* paramName = new string("__p" + i++);
		targetParams->push_back(new ZArg(paramType, paramName));
		newScope->add(new SymbolEntry(paramType, *paramName));
	}

	int j = 0;
	for (ZType* paramType : targetType->getParamTypes()) {
		std::string* paramName = new string("__p" + j++);
		originArgs->push_back(new ZCast(new ZId(*paramName, newScope->makeRef()), paramType, initRef));
	}

	ZCall* callToOriginal = new ZCall(zfunccast->getExpr(), *originArgs, new std::vector<ZType*>, newScope->makeRef());

    ZExpr* body;
    if (targetType->getRetType()->isEqual(*Void))
        body = callToOriginal;
    else
	    body = new ZCast(callToOriginal, zfunccast->getTargetType(), initRef);

	ZLambda* wrapperLambda = new ZLambda(targetParams, targetType->getRetType(), body, initRef);

	parent->replaceChild(zfunccast, wrapperLambda);

	wrapperLambda->accept(this);

}

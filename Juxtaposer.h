#pragma once

#include "TypingPass.h"
#include "ZCall.h"

class Juxtaposer {
public:
	Juxtaposer(TypingPass* typingPass) {
		_typingPass = typingPass;
	}

	void juxtapose(ZType* calleeType, ZCall* call) {
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

	void juxtapose(ZType* paramType, ZExpr* expr, SymbolRef* ref) {
		ZType* type;
		auto lambda = dynamic_cast<ZLambda*>(expr);
		if (lambda) {
			type = juxtapose(paramType, expr->getType(), ref);
			expr->setType(type);
			expr->accept(_typingPass);
			type = juxtapose(paramType, expr->getType(), ref);
		}
		else {
			expr->accept(_typingPass);
			type = juxtapose(paramType, expr->getType(), ref);
		}
	}

	ZType* juxtapose(ZType* paramType, ZType* argType, SymbolRef* ref) {
		ZGenericParam* gen = dynamic_cast<ZGenericParam*>(paramType);
		if (gen) {
			ZType* resolved = ref->resolve(paramType);
			if (!resolved->isEqual(*Unknown)) {
				if (argType->isEqual(*Unknown))
					return resolved;
				if (resolved->isEqual(*argType))
					return argType;
				throw ("Type mismatch: expected " + resolved->toString() + ", but passed " + argType->toString());
			}
			else {
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

private:
	TypingPass* _typingPass;

};

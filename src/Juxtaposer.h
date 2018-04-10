#pragma once

#include "TypingPass.h"
#include "ZCall.h"
#include <set>
#include "ParserError.h"
#include "JuxtaposeResult.h"

class Juxtaposer {
public:
    Juxtaposer(TypingPass* typingPass) {
        _typingPass = typingPass;
    }

    bool juxtapose(ZType* calleeType, ZCall* call) {
        auto funcType = dynamic_cast<ZFuncType*>(calleeType);

        SymbolRef* ref = call->getRef();

        bool isTypeArgsDefined = call->getTypeArgs()->size();
        int i = 0;
        for (ZGenericParam* param : *funcType->getGenericDefs())
            ref->addResolution(param, isTypeArgsDefined ? (*call->getTypeArgs())[i++] : Unknown);

        auto paramTypes = funcType->getParamTypes();
        auto argExprs = call->getArgs();

        std::set<ZExpr*> resolved;
        for (;;) {
            if (resolved.size() == argExprs.size())
                break;
            bool advances = false;
            for (int i = 0; i < argExprs.size(); ++i) {
                auto res = juxtapose(paramTypes[i], argExprs[i], ref);
                if (res.advances)
                    advances = true;
                if (res.completed)
                    resolved.insert(argExprs[i]);
            }
            if (!advances)
                return false;
        }

        call->setType(juxtapose(funcType->getRetType(), call->getType(), ref).type);

        return true;
    }

    JuxtaposeResult juxtapose(ZType* paramType, ZExpr* expr, SymbolRef* ref) {
        JuxtaposeResult res;
        auto lambda = dynamic_cast<ZLambda*>(expr);
        if (lambda) {
            res = juxtapose(paramType, expr->getType(), ref);
            expr->setType(res.type);

            if (allParamTypesKnown(dynamic_cast<ZFuncType*>(res.type))) {
                expr->accept(_typingPass);
                res = juxtapose(paramType, expr->getType(), ref);
            }
        }
        else {
            expr->accept(_typingPass);
            res = juxtapose(paramType, expr->getType(), ref);
        }

        return res;
    }

    bool allParamTypesKnown(ZFuncType* type) {
        for (auto paramType : type->getParamTypes())
            if (paramType->isEqual(*Unknown))
                return false;
        return true;
    }

    JuxtaposeResult juxtapose(ZType* paramType, ZType* argType, SymbolRef* ref) {
        ZGenericParam* gen = dynamic_cast<ZGenericParam*>(paramType);
        if (gen) {
            ZType* resolvedType = ref->resolve(paramType);
            if (!resolvedType->isEqual(*Unknown)) {
                if (argType->isEqual(*Unknown))
                    return JuxtaposeResult(resolvedType, true, true);
                if (resolvedType->isEqual(*argType))
                    return JuxtaposeResult(argType, false, true);
                throw ("Type mismatch: expected " + resolvedType->toString() + ", but passed " + argType->toString());
            }
            else {
                if (argType->isEqual(*Unknown))
                    return JuxtaposeResult(argType, false, false);
                ref->addResolution(gen, argType);
                return JuxtaposeResult(argType, true, true);
            }
        }

        if (argType->isEqual(*Unknown))
            argType = paramType->copyStem();

        bool advances = false;
        bool completed = true;
        for (int i = 0; i < paramType->getTypeParams()->size(); ++i) {
            auto res = juxtapose((*paramType->getTypeParams())[i], (*argType->getTypeParams())[i], ref);
            if (res.advances)
                advances = true;
            if (!res.completed)
                completed = false;
            argType->setTypeParam(i, res.type);
        }

        return JuxtaposeResult(argType, advances, completed);
    }

    void addError(ParserError* error) {
        _errors.push_back(error);
    }

    std::vector<ParserError*> getErrors() {
        return _errors;
    }

private:
    TypingPass* _typingPass;
    std::vector<ParserError*> _errors;
};

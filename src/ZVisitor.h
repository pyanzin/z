﻿#pragma once
#include <string>
#include "SourceRange.h"
#include "ParserError.h"
#include <vector>
#include "RecoveryException.h"

class ZFunc;
class ZAst;
class ZArg;
class ZModule;
class ZClassDef;
class ZBlock;
class ZBinOp;
class ZId;
class ZAssign;
class ZCall;
class ZVarDef;
class ZReturn;
class ZIf;
class ZWhile;
class ZSubscript;
class ZLambda;
class ZFor;
class ZSelector;
class ZCast;
class ZUnaryOp;

class ZVisitor {
public:
    virtual void visit(ZAst* zast) {
    }

    virtual void visit(ZModule* zmodule) {
    }

    virtual void visit(ZFunc* zfunc) {
    }

    virtual void visit(ZClassDef* zclassdef) {
    }

    virtual void visit(ZArg* zarg) {
    }

    virtual void visit(ZBlock* zblock) {
    }

    virtual void visit(ZVarDef* zvardef) {
    }

    virtual void visit(ZCast* zcast) {
    }

    virtual void visit(ZAssign* zassign) {
    }

    virtual void visit(ZBinOp* zbinop) {
    }

    virtual void visit(ZUnaryOp* zunaryop) {
    }

    virtual void visit(ZSelector* zselector) {
    }

    virtual void visit(ZLambda* zlambda) {
    }

    virtual void visit(ZCall* zcall) {
    }

    virtual void visit(ZId* zid) {
    }

    virtual void visit(ZReturn* zreturn) {
    }

    virtual void visit(ZIf* zif) {
    };

    virtual void visit(ZWhile* zwhile) {
    };

    virtual void visit(ZFor* zfor) {
    };

    virtual void visit(ZSubscript* zsubscript) {
    };

    void error(std::string text, SourceRange* sr) {
        errors.push_back(new ParserError(text, sr));
        throw RecoveryException();
    }

    std::vector<ParserError*> getErrors() {
        return errors;
    }

private:
    std::vector<ParserError*> errors;
};

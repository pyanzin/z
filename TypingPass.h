﻿#pragma once
#include "ZVisitor.h"
#include "ZExpr.h"
#include "ZLambda.h"
#include "ZSelector.h"
#include "ZCast.h"
#include "ZFuncCast.h"

class SymbolRef;
class ZIf;

class TypingPass : public ZVisitor {
public:
	void visit(ZModule* zmodule) override;

	void visit(ZFunc* zfunc) override;

	void visit(ZBlock* zblock) override;

	void visit(ZVarDef* zvardef) override;
	void visit(ZCast* zcast) override;
    void visit(ZFuncCast* zfunccast);
    void visit(ZAssign* zassign) override;
    void visit(ZLambda* zlambda) override;
    void visit(ZCall* zcall) override;

	void visit(ZBinOp* zbinop) override;
	void visit(ZSelector* zselector) override;
	void visit(ZId* zid) override;

	void visit(ZReturn* zreturn) override;

	void visit(ZIf* zif) override;

	void visit(ZWhile* zwhile) override;

	void visit(ZFor* zfor) override;

    void visit(ZSubscript* zsubscript) override;

private:
	ZFunc* _func;
    ZModule* _module;
};

#pragma once
#include "ZVisitor.h"
#include "ZVarDef.h"

class TypingPass : public ZVisitor {
public:
	void visit(ZModule* zmodule) override;

	void visit(ZFunc* zfunc) override;

	void visit(ZBlock* zblock) override;

	void visit(ZVarDef* zvardef) override;

	void visit(ZAssign* zassign) override;

	void visit(ZCall* zcall) override;

	void visit(ZBinOp* zbinop) override;

    void visit(ZId* zid) override;

	void visit(ZReturn* zreturn) override;

};

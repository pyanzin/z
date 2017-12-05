#pragma once
#include <string>

class ZFunc;
class ZAst;
class ZArg;
class ZModule;
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
	virtual void visit(ZAst* zast) { }

	virtual void visit(ZModule* zmodule) { }

	virtual void visit(ZFunc* zfunc) { }

	virtual void visit(ZArg* zarg) { }

	virtual void visit(ZBlock* zblock) { }

	virtual void visit(ZVarDef* zvardef) { }

	virtual void visit(ZCast* zcast) { }

	virtual void visit(ZAssign* zassign) { }		

	virtual void visit(ZBinOp* zbinop) { }

	virtual void visit(ZUnaryOp* zunaryop) { }

	virtual void visit(ZSelector* zselector) { }

    virtual void visit(ZLambda* zlambda) { }

	virtual void visit(ZCall* zcall) { }

	virtual void visit(ZId* zid) { }

	virtual void visit(ZReturn* zreturn) { }

	virtual void visit(ZIf* zif) { };

	virtual void visit(ZWhile* zwhile) { };

	virtual void visit(ZFor* zfor) { };

    virtual void visit(ZSubscript* zsubscript) { };

	void error(std::string text) {
		throw std::exception(text.c_str());
	}

	void error(std::string text, std::string position) {
		error(text + " at " + position);
	}
};

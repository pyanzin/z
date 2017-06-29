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

class ZVisitor {
public:
	virtual void visit(ZAst* zast) { }

	virtual void visit(ZModule* zmodule) { }

	virtual void visit(ZFunc* zfunc) { }

	virtual void visit(ZArg* zarg) { }

	virtual void visit(ZBlock* zblock) { }

	virtual void visit(ZAssign* zassign) { }		

	virtual void visit(ZBinOp* zbinop) { }

	virtual void visit(ZCall* zcall) { }

	virtual void visit(ZId* zid) { }

	void error(std::string& text) {
		throw std::exception(text.c_str());
	}
};

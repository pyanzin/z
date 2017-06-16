#pragma once
#include "ZVar.h"

class ZFunc;
class ZAst;
class ZArg;
class ZModule;

class ZVisitor {
public:
    ZAst* visit(ZAst* ast);

    ZAst* visit(ZArg* ast);

	virtual void visit(ZFunc* func) { }

	virtual void visit(ZModule* module) { }

	virtual void visit(ZVar* zmodule) { }
};

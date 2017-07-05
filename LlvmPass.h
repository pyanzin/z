#pragma once
#include <llvm/IR/Module.h>
#include "ZVisitor.h"
#include <llvm/IR/IRBuilder.h>
#include "ZIntLit.h"

class ZExpr;

class LlvmPass : public ZVisitor
{
public:
	LlvmPass();

	void visit(ZFunc* zfunc) override;

	void visit(ZModule* zmodule) override;

	void visit(ZBlock* zblock) override;

	void visit(ZVarDef* zvardef) override;
	void visit(ZReturn* zreturn) override;
	void visit(ZBinOp* zbinop) override;

	llvm::Value* getValue(ZExpr* zexpr);

	llvm::Value* getValue(ZId* zvar);

	llvm::Value* getValue(ZBinOp* zbinop);
	llvm::Value* getValue(ZIntLit* zintlit);
	llvm::Value* getValue(ZCall* zcall);

	llvm::Module* getModule() {
		return _module;
	}
private:
	llvm::Module* _module;
	llvm::Function* _func;
	std::map<std::string, llvm::Value*> _currentValues;

	llvm::IRBuilder<>* _builder;
};

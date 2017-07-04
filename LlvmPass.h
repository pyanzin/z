#pragma once
#include <llvm/IR/Module.h>
#include "ZVisitor.h"
#include <llvm/IR/IRBuilder.h>

class LlvmPass : public ZVisitor
{
public:
	LlvmPass();

	void visit(ZFunc* zfunc) override;

	void visit(ZModule* zmodule) override;

	void visit(ZBlock* zblock) override;

	void visit(ZVarDef* zvardef) override;

	void visit(ZBinOp* zbinop) override;

	llvm::Value* getValue(ZId* zvar);

	llvm::Module* getModule() {
		return _module;
	}
private:
	llvm::Module* _module;
	llvm::Function* _func;
	std::map<std::string, llvm::Value*> _currentValues;

	llvm::IRBuilder<>* _builder;
};
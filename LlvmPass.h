#pragma once
#include <llvm/IR/Module.h>
#include "ZVisitor.h"


class LlvmPass : public ZVisitor
{
public:
	LlvmPass() {
		//_builder((getGlobalContext());
	}

	void visit(ZFunc* zfunc) override;

	void visit(ZModule* zmodule) override;

	void visit(ZId* zvar) override {
		
	}

private:
	llvm::Module* _module;
	llvm::Function* _func;
	std::map<std::string, llvm::Value*> _currentValues;

	//IRBuilder<>& _builder;
};


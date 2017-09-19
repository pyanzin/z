#pragma once
#include <llvm/IR/Module.h>
#include "ZVisitor.h"
#include <llvm/IR/IRBuilder.h>
#include "ZIntLit.h"
#include "LlvmTable.h"
#include "ZCharLit.h"
#include "ZStringLit.h"
#include "ZBooleanLit.h"
#include "ZNop.h"

class ZExpr;

class LlvmPass : public ZVisitor
{
public:
	LlvmPass();

	void visit(ZFunc* zfunc) override;

	void visit(ZModule* zmodule) override;
	void addFuncDef(ZFunc* zfunc);
	llvm::BasicBlock* generate(ZBlock* zblock);
	llvm::BasicBlock* generate(ZAst* zast);

	llvm::BasicBlock* generate(ZVarDef* zvardef);
	llvm::BasicBlock* generate(ZReturn* zreturn);
	llvm::BasicBlock* generate(ZIf* zif);
	llvm::BasicBlock* generate(ZWhile* zwhile);
	BasicBlock* generate(ZNop* znop);
	llvm::Value* getValue(ZExpr* zexpr, llvm::BasicBlock* bb);

	llvm::Value* getValue(ZId* zvar);

	llvm::Value* getValue(ZBinOp* zbinop, llvm::BasicBlock* bb);
	Value* getValue(ZStringLit* zstringlit);
	llvm::Value* getValue(ZIntLit* zintlit);
	Value* getValue(ZCharLit* zintlit);
    Value* getValue(ZBooleanLit* zbooleanlit);
    llvm::Value* getValue(ZCall* zcall, BasicBlock* bb);
	llvm::Value* getValue(ZAssign* zassign, llvm::BasicBlock* bb);
	Value* getValue(ZFunc* zfunc);
	BasicBlock* makeBB(std::string name);
    BasicBlock* makeNopBB(std::string name);

    llvm::Module* getModule() {
		return _module;
	}
private:
	llvm::Module* _module;
	llvm::Function* _func;
    llvm::BasicBlock* _lastBB;
	LlvmTable* _currentValues;

    llvm::IRBuilder<>* _builder;
};

#include "LlvmPass.h"
#include <llvm/IR/LLVMContext.h>
#include "ZFunc.h"
#include "ZModule.h"
#include "ZArg.h"
#include "ZBasicTypes.h"
#include "ZBlock.h"
#include "ZId.h"
#include "ZBinOp.h"
#include "ZVarDef.h"
#include "ZIntLit.h"
#include "ZCall.h"
#include "ZReturn.h"
#include "llvm/IR/BasicBlock.h"

void LlvmPass::visit(ZModule* zmodule) {
	_module = new llvm::Module("test", llvm::getGlobalContext());
	for (ZFunc* func : zmodule->getFunctions()) {
		func->accept(this);
	}
}

void LlvmPass::visit(ZFunc* zfunc) {
	_currentValues.clear();

	auto args = std::vector<llvm::Type*>();
	for (auto arg : zfunc->_args)
		args.push_back(arg->getType()->toLlvmType());

	auto funcType = llvm::FunctionType::get(zfunc->_returnType->toLlvmType(), args, false);
	_func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, zfunc->_name->c_str(), _module);

	auto zargs = zfunc->_args;
	unsigned i = 0;
	for (auto& arg : _func->args()) {
		auto zarg = zargs[i++];
		arg.setName(*zarg->getName());
		_currentValues[*zarg->getName()] = &arg;
	}

	_currentValues[*zfunc->_name] = _func;

	generate(zfunc->_body);
}

llvm::BasicBlock* LlvmPass::generate(ZBlock* zblock) {
	llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _func);

	for (ZAst* stmt : zblock->getStatements()) {
		llvm::BasicBlock* newBB = llvm::BasicBlock::Create(llvm::getGlobalContext());
		generate(stmt);
	}

	return bb;
}

llvm::BasicBlock* LlvmPass::generate(ZAst* zast) {
	llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _func);

	ZExpr* zexpr = dynamic_cast<ZExpr*>(zast);
	if (zexpr) {
		getValue(zexpr, bb);
		return bb;
	}

	ZVarDef* zvardef = dynamic_cast<ZVarDef*>(zast);
	if (zvardef) 
		return generate(zvardef);
	
	ZReturn* zreturn = dynamic_cast<ZReturn*>(zast);
	if (zreturn)
		return generate(zreturn);
}

llvm::BasicBlock* LlvmPass::generate(ZVarDef* zvardef) {
	llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _func);

	llvm::Value* init = getValue(zvardef->getInitExpr(), bb);
	_currentValues[zvardef->getName()] = init;

	return bb;
}

llvm::BasicBlock* LlvmPass::generate(ZReturn* zreturn) {
	llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _func);

	auto retValue = getValue(zreturn->getExpr(), bb);
	_builder->CreateRet(retValue);

	return bb;
}

LlvmPass::LlvmPass() {
	_builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
}


llvm::Value* LlvmPass::getValue(ZExpr* zexpr, llvm::BasicBlock* bb) {
	ZCall* zcall = dynamic_cast<ZCall*>(zexpr);
	if (zcall)
		return getValue(zcall, bb);

	ZId* zid = dynamic_cast<ZId*>(zexpr);
	if (zid)
		return getValue(zid);

	ZBinOp* zbinop = dynamic_cast<ZBinOp*>(zexpr);
	if (zbinop)
		return getValue(zbinop, bb);

	ZIntLit* zintlit = dynamic_cast<ZIntLit*>(zexpr);
	if (zintlit)
		return getValue(zintlit);
}

llvm::Value* LlvmPass::getValue(ZId* zid) {
	return _currentValues[zid->getName()];
}

llvm::Value* LlvmPass::getValue(ZBinOp* zbinop, llvm::BasicBlock* bb) {
	_builder->SetInsertPoint(bb);
	auto left = getValue(zbinop->getLeft(), bb);
	auto right = getValue(zbinop->getRight(), bb);

	switch (zbinop->getOp()) {
	case Sum:
		return _builder->CreateAdd(left, right);
	case Sub:
		return _builder->CreateSub(left, right);
	case Mul:
		return _builder->CreateMul(left, right);
	case Div:
		return _builder->CreateSDiv(left, right);
	case Equal:
		return _builder->CreateICmpEQ(left, right);
	default:
		return nullptr;
	}
}

llvm::Value* LlvmPass::getValue(ZIntLit* zintlit) {
	return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::APInt(32, zintlit->getValue()));
}

llvm::Value* LlvmPass::getValue(ZCall* zcall, llvm::BasicBlock* bb) {
	auto callee = getValue(zcall->callee, bb);

	auto args = new std::vector<llvm::Value*>();
	for (auto arg : zcall->getArgs())
		args->push_back(getValue(arg, bb));
	return _builder->CreateCall(callee, *args, "");
}
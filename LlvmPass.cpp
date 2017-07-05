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
//#include "llvm/IR/IRBuilder.h"

void LlvmPass::visit(ZModule* zmodule) {
	_module = new llvm::Module("test", llvm::getGlobalContext());
	for (ZFunc* func : zmodule->getFunctions()) {
		func->accept(this);
	}
}

void LlvmPass::visit(ZBlock* zblock) {
	for (ZAst* stmt : zblock->_expressions)
		stmt->accept(this);
}

void LlvmPass::visit(ZBinOp* zbinop) {
	getValue(zbinop);
}

void LlvmPass::visit(ZVarDef* zvardef) {
	llvm::Value* init = getValue(zvardef->getInitExpr());
	_currentValues[zvardef->getName()] = init;
}

LlvmPass::LlvmPass() {
	_builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
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

	zfunc->_body->accept(this);
}


llvm::Value* LlvmPass::getValue(ZExpr* zexpr) {
	ZCall* zcall = dynamic_cast<ZCall*>(zexpr);
	if (zcall)
		return getValue(zcall);

	ZId* zid = dynamic_cast<ZId*>(zexpr);
	if (zid)
		return getValue(zid);

	ZBinOp* zbinop = dynamic_cast<ZBinOp*>(zexpr);
	if (zbinop)
		return getValue(zbinop);

	ZIntLit* zintlit = dynamic_cast<ZIntLit*>(zexpr);
	if (zintlit)
		return getValue(zintlit);
}

llvm::Value* LlvmPass::getValue(ZId* zid) {
	return _currentValues[zid->getName()];
}

llvm::Value* LlvmPass::getValue(ZBinOp* zbinop) {
	llvm::BasicBlock* bb = llvm::BasicBlock::Create(llvm::getGlobalContext(), "", _func);
	_builder->SetInsertPoint(bb);
	switch (zbinop->getOp()) {
	case Sum:
		return _builder->CreateAdd(getValue(zbinop->getLeft()), getValue(zbinop->getRight()));
	case Sub:
		return _builder->CreateSub(getValue(zbinop->getLeft()), getValue(zbinop->getRight()));
	case Mul:
		return _builder->CreateMul(getValue(zbinop->getLeft()), getValue(zbinop->getRight()));
	case Div:
		return _builder->CreateSDiv(getValue(zbinop->getLeft()), getValue(zbinop->getRight()));
	default:
		return nullptr;
	}
}

llvm::Value* LlvmPass::getValue(ZIntLit* zintlit) {
	return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::APInt(32, zintlit->getValue()));
}

llvm::Value* LlvmPass::getValue(ZCall* zcall) {
	auto callee = getValue(zcall->callee);

	auto args = new std::vector<llvm::Value*>();
	for (auto arg : zcall->getArgs())
		args->push_back(getValue(arg));
	return _builder->CreateCall(callee, *args, "");
}
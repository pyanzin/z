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
	//switch (zbinop->getOp())
	//{
	//case Sum:
	//	return _builder->CreateAdd(zbinop->getLeft()->codegen(_func), _b->codegen(_func));
	//case Sub:
	//	return builder.CreateSub(_a->codegen(func), _b->codegen(func));
	//case Mul:
	//	return builder.CreateMul(_a->codegen(func), _b->codegen(func));
	//case Div:
	//	return builder.CreateSDiv(_a->codegen(func), _b->codegen(func));
	//default:
	//	return nullptr;
	//}
}

void LlvmPass::visit(ZVarDef* zvardef) {
		
	
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

	zfunc->_body->accept(this);
}

llvm::Value* LlvmPass::getValue(ZId* zid) {
	return _currentValues[zid->getName()];
}


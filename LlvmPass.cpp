#include "LlvmPass.h"
#include <llvm/IR/LLVMContext.h>
#include "ZFunc.h"
#include "ZModule.h"
#include "ZArg.h"
#include "ZBasicTypes.h"

void LlvmPass::visit(ZFunc* zfunc) {
	auto args = std::vector<llvm::Type*>();
	for (auto arg : zfunc->_args)
		args.push_back(arg->getType()->toLlvmType());

	auto funcType = llvm::FunctionType::get(zfunc->_returnType->toLlvmType(), args, false);
	_func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, zfunc->_name->c_str(), _module);

	auto zargs = zfunc->_args;
	unsigned i = 0;
	for (auto& arg : _func->args())
		arg.setName(zargs[i++]->getName().c_str());

	zfunc->_body->accept(this);
}

void LlvmPass::visit(ZModule* zmodule) {
	_module = new llvm::Module("test", llvm::getGlobalContext());
	for (ZFunc* func : zmodule->getFunctions()) {
		func->accept(this);
	}
}

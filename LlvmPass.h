#pragma once
#include "ZVisitor.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include "ZFunc.h"
#include "ZVar.h"
#include "ZModule.h"
#include "Utils.h"

class LlvmPass : public ZVisitor
{
public:
	LlvmPass() {
		//_builder((getGlobalContext());
	}

	virtual void visit(ZFunc* zfunc) override {
		auto args = std::vector<Type*>();
		for (auto arg : zfunc->_args)
			args.push_back(toLlvmType(arg->getType()));

		auto funcType = FunctionType::get(toLlvmType(zfunc->_returnType), args, false);
		_func = Function::Create(funcType, Function::ExternalLinkage, zfunc->_name->c_str(), _module);

		auto zargs = zfunc->_args;
		unsigned i = 0;
		for (auto& arg : _func->args())
			arg.setName(zargs[i++]->getName()->c_str());

		zfunc->_body->accept(this);
	}

	virtual void visit(ZModule* zmodule) override {
		_module = new llvm::Module("test", llvm::getGlobalContext());
		for (ZFunc* func : zmodule->getFunctions()) {
			func->accept(this);
		}
	}

	virtual void visit(ZId* zvar) override {
		for (auto bb = _func->begin(); bb != _func->end(); ++bb) {
			for (auto inst = bb->begin(); inst != bb->end(); ++inst)
				if (inst->getName() == zvar->getName())
					;
		}
	}

private:
	llvm::Module* _module;
	llvm::Function* _func;

	//IRBuilder<>& _builder;
};


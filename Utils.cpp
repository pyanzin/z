#include "ZBasicTypes.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>

llvm::Type* toLlvmType(BaseTypes ztype) {
	switch (ztype)
	{
	case Int:
		return llvm::IntegerType::get(llvm::getGlobalContext(), 32);
	case Double:
		break;
	case Boolean:
		return llvm::IntegerType::get(llvm::getGlobalContext(), 1);
	case String:
	default:
		return nullptr;
	}
}

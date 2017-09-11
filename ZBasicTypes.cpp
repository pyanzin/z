#include "ZBasicTypes.h"
#include <llvm/IR/Function.h>

inline llvm::Type* ZBasicType::toLlvmType() {
	return _type;
}

inline std::string& ZBasicType::toString() {
	return _name;
}

llvm::Type* ZFuncType::toLlvmType() {
	std::vector<llvm::Type*>* argTypes = new std::vector<llvm::Type*>();
	for (auto argType : _argTypes)
		argTypes->push_back(argType->toLlvmType());

	auto funcType = llvm::FunctionType::get(_retType->toLlvmType(), argTypes);
	return funcType;
}

// todo: fill all basic types
ZBasicType* Unknown = (new ZBasicType(llvm::Type::getVoidTy(llvm::getGlobalContext()), "Unknown"));
ZBasicType* Int = (new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 32), "Int"));
ZBasicType* Char = (new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 8), "Char"));
ZBasicType* Double = (new ZBasicType(llvm::Type::getFloatTy(llvm::getGlobalContext()), "Double"));
ZBasicType* Boolean = (new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 1), "Boolean"));
ZBasicType* String = (new ZBasicType(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), "String"));
ZBasicType* Void = (new ZBasicType(llvm::Type::getVoidTy(llvm::getGlobalContext()), "None"));
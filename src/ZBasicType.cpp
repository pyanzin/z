#include "ZBasicType.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include "ZArrayType.h"
#include "ZWildcardType.h"
#include "ZNumberType.h"

inline llvm::Type* ZBasicType::toLlvmType() {
	return _type;
}

inline std::string& ZBasicType::toString() {
	return _name;
}

// todo: fill all basic types
ZType* Void = new ZBasicType(llvm::Type::getVoidTy(getLlvmContext()), "None");
ZType* Any = new ZBasicType(llvm::IntegerType::get(getLlvmContext(), 8), "Any");
ZType* Unknown = new ZBasicType(llvm::Type::getVoidTy(getLlvmContext()), "Unknown");


ZType* Double = new ZNumberType(llvm::Type::getDoubleTy(getLlvmContext()), "Double", *new std::vector<ZType*>());
ZType* Int = new ZNumberType(llvm::IntegerType::get(getLlvmContext(), 32), "Int", *new std::vector<ZType*>() = {Double});
ZType* Char = new ZNumberType(llvm::IntegerType::get(getLlvmContext(), 8), "Char", *new std::vector<ZType*>() = { Double, Int });
ZType* Boolean = new ZBasicType(llvm::IntegerType::get(getLlvmContext(), 1), "Boolean");
ZType* String = new ZBasicType(llvm::Type::getInt8PtrTy(getLlvmContext()), "String");

ZWildcardType* Wildcard = new ZWildcardType();

ZBasicType* Ref = new ZBasicType(llvm::PointerType::get(llvm::IntegerType::get(getLlvmContext(), 8), 0), "Ref");
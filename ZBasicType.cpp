#include "ZBasicType.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include "ZArrayType.h"
#include "ZWildcardType.h"

inline llvm::Type* ZBasicType::toLlvmType() {
	return _type;
}

inline std::string& ZBasicType::toString() {
	return _name;
}

// todo: fill all basic types
ZBasicType* Void = new ZBasicType(llvm::Type::getVoidTy(llvm::getGlobalContext()), "None");
ZBasicType* Any = new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 8), "Any");
ZBasicType* Unknown = new ZBasicType(llvm::Type::getVoidTy(llvm::getGlobalContext()), "Unknown");

ZBasicType* Int = new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 32), "Int");
ZBasicType* Char = new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 8), "Char");
ZBasicType* Double = new ZBasicType(llvm::Type::getFloatTy(llvm::getGlobalContext()), "Double");
ZBasicType* Boolean = new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 1), "Boolean");
ZBasicType* String = new ZBasicType(llvm::Type::getInt8PtrTy(llvm::getGlobalContext()), "String");

ZWildcardType* Wildcard = new ZWildcardType();

ZBasicType* Ref = new ZBasicType(llvm::PointerType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 8), 0), "Ref");
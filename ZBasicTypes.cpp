#include "ZBasicTypes.h"

inline llvm::Type* ZBasicType::toLlvmType() {
	return _type;
}

inline std::string& ZBasicType::toString() {
	return _name;
}

// todo: fill all basic types
ZBasicType* Unknown = (new ZBasicType(llvm::Type::getVoidTy(llvm::getGlobalContext()), "Unknown"));
ZBasicType* Int = (new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 32), "Int"));
ZBasicType* Double = (new ZBasicType(llvm::Type::getFloatTy(llvm::getGlobalContext()), "Double"));
ZBasicType* Boolean = (new ZBasicType(llvm::IntegerType::get(llvm::getGlobalContext(), 1), "Boolean"));
ZBasicType* String = (new ZBasicType(llvm::ArrayType::get(llvm::Type::getInt16Ty(llvm::getGlobalContext()), 0), "String"));
ZBasicType* None = (new ZBasicType(llvm::Type::getVoidTy(llvm::getGlobalContext()), "None"));
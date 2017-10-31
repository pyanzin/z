#pragma once

#include "ZType.h"
#include <string>
#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>

class ZWildcardType : public ZType {

public:
	virtual llvm::Type* toLlvmType() {
		return llvm::IntegerType::get(llvm::getGlobalContext(), 8);
	}

	virtual std::string& getName() {
		return *new std::string("_");
	}

	virtual std::string& toString() {
		return *new std::string("Wildcard");
	}

	bool isEqual(ZType& other) override {
		return dynamic_cast<ZWildcardType*>(&other);
	}
};

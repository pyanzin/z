#pragma once
#include "ZType.h"
#include "ZGenericParam.h"
#include "ZArg.h"
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>

class ZStructType : public ZType {
public:
	ZStructType(std::string* name, std::vector<ZArg*>* members, std::vector<ZGenericParam*>* genericDefs = nullptr) {
		_name = name;
		_members = members;
		if (genericDefs)
			for (ZGenericParam* gen : *genericDefs)
				_typeParams->push_back(gen);			
	}

	llvm::Type* toLlvmType() override {
		auto memberLlvmTypes = new std::vector<llvm::Type*>();
		for (ZArg* member : *_members)
			memberLlvmTypes->push_back(member->getType()->toLlvmType());

		llvm::StructType* type = llvm::StructType::get(llvm::getGlobalContext(), memberLlvmTypes);

		return llvm::PointerType::get(type, 0);
	}

	std::string& getName() override {
		return *_name;
	}

	std::string& toString() override {
		return *_name;
	}

	bool isEqual(ZType& other) override {
		return this == &other;
	}
private:
	std::vector<ZArg*>* _members;
	std::string* _name;
};

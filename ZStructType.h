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
		_structType = nullptr;
		if (genericDefs)
			for (ZGenericParam* gen : *genericDefs)
				_typeParams->push_back(gen);			
	}

	llvm::StructType* getStructType() {
		auto memberLlvmTypes = new std::vector<llvm::Type*>();
		for (ZArg* member : *_members)
			memberLlvmTypes->push_back(member->getType()->toLlvmType());

		auto type = llvm::StructType::create(llvm::getGlobalContext(), *memberLlvmTypes);
		return type;
	}

	llvm::Type* toLlvmType() override {
		if (_structType)
			return _structType;

		llvm::StructType* type = getStructType();
		_structType = llvm::PointerType::get(type, 0);

		return _structType;
	}

	int getSize() {
		return getStructType()->getPrimitiveSizeInBits() / 8;
	}

	int indexOfMember(std::string& name) {
		for (int i = 0; i < _members->size(); ++i)
			if (*(*_members)[i]->getName() == name)
				return i;
		return -1;
	}

	ZArg* getMember(string& name) {
		for (int i = 0; i < _members->size(); ++i)
			if (*(*_members)[i]->getName() == name)
				return (*_members)[i];
		return nullptr;
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

	llvm::Type* _structType;
};

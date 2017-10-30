#pragma once
#include <llvm/IR/Type.h>
#include "ZType.h"

class ZArrayType : public ZType {
public:
	ZArrayType(ZType* elemType) {
        _typeParams->push_back(elemType);
	}

	ZType* getElementType() {
		return (*_typeParams)[0];
	}

	llvm::Type* toLlvmType() override {
		return (llvm::Type*)(*_typeParams)[0]->toLlvmType()->getPointerTo();
	}

	std::string& toString() override {
	    auto elemTypeName = (*_typeParams)[0] ? (*_typeParams)[0]->toString() : "_";
	    return *(new std::string("Array[" + elemTypeName + "]"));
    }

    std::string& getName() override {
		return *(new std::string("Array"));
	}

	bool isEqual(ZType& other) override {
		auto otherArray = dynamic_cast<ZArrayType*>(&other);
		if (otherArray && otherArray->getElementType()->isEqual(*getElementType()))
			return true;
		return false;		
	}
};

﻿#pragma once
#include <llvm/IR/Type.h>
#include "ZType.h"

class ZArrayType : public ZType {
public:
	ZArrayType(ZType* elemType) {
		_elementType = elemType;
	}

	ZType* getElementType() {
		return  _elementType;
	}

	llvm::Type* toLlvmType() override {
		return (llvm::Type*)_elementType->toLlvmType()->getPointerTo();
	}

	std::string& toString()  override {
	    auto elemTypeName = _elementType ? _elementType->toString() : "_";
	    return *(new std::string("Array[" + elemTypeName + "]"));
    }

    std::string& getName() override {
		return *(new std::string("Array"));
	}

	bool isEqual(ZType& other)  override  {
		auto otherArray = dynamic_cast<ZArrayType*>(&other);
		if (otherArray && otherArray->getElementType()->isEqual(*getElementType()))
			return true;
		return false;		
	}
private:
	ZType* _elementType;
};

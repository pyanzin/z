#pragma once
#include "ZBasicTypes.h"
#include <llvm/IR/Type.h>

class ZArrayType : public ZType {
	ZArrayType(ZType* elemType) {
		_elementType = elemType;
	}

	ZType* getElementType() {
		return  _elementType;
	}

	llvm::Type* toLlvmType() {
		return getElementType()->toLlvmType()->getPointerTo();
	}

private:
	ZType* _elementType;

};

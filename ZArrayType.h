#include "ZBasicTypes.h"

class ZArrayType : public ZType {
	ZArrayType(ZType* elemType) {
		_elementType = elemType;
	}

	ZType* getElementType() {
		return  _elementType;
	}

private:
	ZType* _elementType;

};

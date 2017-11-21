#include "ZType.h"
#include "ZBasicType.h"
#include "ZGenericParam.h"

bool ZType::canBeCastedTo(ZType* other) {
	if (other->isEqual(*Wildcard))
		return true;
	return false;
}

ZType* ZType::copyStem() {
    return this;
}

bool ZType::containsGenerics() {
	if (dynamic_cast<ZGenericParam*>(this))
		return true;
	for (ZType* tp : *_typeParams)
		if (tp->containsGenerics())
			return true;
	return false;
}

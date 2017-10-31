#include "ZType.h"
#include "ZBasicType.h"

bool ZType::canBeCastedTo(ZType& other) {
	if (other.isEqual(*Wildcard))
		return true;
	return false;
}

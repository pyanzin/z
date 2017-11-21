#include "SymbolRef.h"

SymbolRef* SymbolScope::makeRef() {
	int id = incrementNumber();
	return new SymbolRef(this, id);
}

#include "ZVisitor.h"
#include "ZArg.h"

void ZArg::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

#include "ZCall.h"
#include "ZVisitor.h"

void ZCall::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

#include "ZAssign.h"
#include "ZVisitor.h"

void ZAssign::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

#include "ZStringLit.h"
#include "ZVisitor.h"

void ZStringLit::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

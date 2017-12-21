#include "ZAst.h"
#include "ZVisitor.h"

void ZAst::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

#include "ZAst.h"

void ZAst::accept(ZVisitor* visitor) {
    visitor->visit(this);
}

#include "ZVarDef.h"
#include "ZVisitor.h"

void ZVarDef::accept(ZVisitor* visitor) {
    visitor->visit(this);
}

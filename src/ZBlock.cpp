#include "ZVisitor.h"
#include "ZBlock.h"

void ZBlock::accept(ZVisitor* visitor) {
    visitor->visit(this);
}

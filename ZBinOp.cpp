#include "ZBinOp.h"
#include "ZBasicTypes.h"

void ZBinOp::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

void ZBinOp::dump(std::ostream& stream, unsigned depth) {
	dumpTab(stream, depth);
	stream << toString(_op) << " : " << getType()->toString() << "\n";
	_left->dump(stream, depth + 1);
	_right->dump(stream, depth + 1);
}

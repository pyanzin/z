#include "ZArg.h"
#include "ZFunc.h"
#include "ZVisitor.h"
#include "ZBasicTypes.h"

void ZFunc::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

void ZFunc::dump(std::ostream& stream, unsigned depth) {
	dumpTab(stream, depth);

	stream << "func " << _name->c_str() << "(";
	for (ZArg* arg : _args) {
		stream << arg->getName() << ": " << arg->getType()->toString() << ", ";
	}

	stream << ")\n";
	_body->dump(stream, depth + 1);
}

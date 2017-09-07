#include "ZArg.h"
#include "ZFunc.h"
#include "ZVisitor.h"
#include "ZBasicTypes.h"

void ZFunc::accept(ZVisitor* visitor) {
	visitor->visit(this);
}

void ZFunc::dump(std::ostream& stream, unsigned depth) {
	dumpTab(stream, depth);

	if (isExtern())
		stream << "extern ";	

	stream << "func " << _name->c_str() << "(";
	for (auto i = _args.begin(); i < _args.end(); ++i) {
		bool isLast = i == _args.end() - 1;
		stream << *((*i)->getName()) << ": " << (*i)->getType()->toString();
		if (!isLast)
			stream << ", ";
	}

	stream << ")\n";

	if (!isExtern())
		_body->dump(stream, depth + 1);	
}

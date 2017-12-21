#pragma once
#include "ZAst.h"

class ZNop : public ZAst {
	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "nop\n";
	}
};
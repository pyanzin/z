#pragma once
#include "ZExpr.h"
#include "Utils.h"

class ZStringLit : public ZExpr {
public:
	ZStringLit(std::string& value) : _value(value) {
		setType(String);
	}

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "string(\'" << _value.c_str() << "\') : " << toString(getType()) << "\n";
	}

	std::string& _value;
};

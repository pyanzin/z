#pragma once
#include "ZExpr.h"
#include "ZBasicTypes.h"

class ZStringLit : public ZExpr {
public:
	ZStringLit(std::string& value) : _value(value) {
		setType(String);
	}

	virtual void accept(ZVisitor* visitor);

	std::string& getValue() {
		return _value;
	}

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "string(\'" << _value << "\') : " << getType()->toString() << "\n";
	}

	std::string& _value;
};

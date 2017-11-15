#pragma once
#include "ZExpr.h"
#include "ZStructType.h"

class ZSelector : public ZExpr {
public:
	ZSelector(ZExpr* target, std::string* member) {
		adopt(target);
		_target = target;
		_member = member;
	}

	void accept(ZVisitor* visitor) override {
		visitor->visit(this);
	}

	ZExpr* getTarget() {
		return _target;
	}

	string* getMember() {
		return _member;
	}

	int getMemberIndex() {
		return dynamic_cast<ZStructType*>(_target->getType())->indexOfMember(*_member);
	}
private:
	ZExpr* _target;
	string* _member;
};

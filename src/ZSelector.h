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

	void replaceChild(ZAst* oldChild, ZAst* newChild) override {
		adopt(newChild);
		if (_target == oldChild) {
			_target = static_cast<ZExpr*>(newChild);
			return;
		}
		
		throw exception("wrong call to replaceChild in ZSelector");
	}

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << "selector : ";
        stream << getType()->toString() << "\n";
        _target->dump(stream, depth + 1);

        dumpTab(stream, depth + 1);
        stream << *getMember() << "\n";
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

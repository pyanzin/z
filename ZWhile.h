#pragma once
#include "ZAst.h"
#include "ZExpr.h"
#include "ZVisitor.h"

class ZWhile : public ZAst {
public:
	ZWhile(ZExpr* cond, ZAst* body) {
		_cond = cond;
		_body = body;
	}

	void accept(ZVisitor* visitor) override {
		visitor->visit(this);
	}

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);
        if (_cond == oldChild) {
            _cond = static_cast<ZExpr*>(newChild);
            return;
        }

        if (_body == oldChild) {
            _body = static_cast<ZExpr*>(newChild);
            return;
        }

        throw exception("wrong call to replaceChild in ZWhile");
    }

	ZExpr* getCondition() {
		return _cond;
	}

	ZAst* getBody() {
		return _body;
	}

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "while\n";
		_cond->dump(stream, depth + 1);

		dumpTab(stream, depth);
		stream << "do\n";
		_body->dump(stream, depth + 1);
	}
private:
	ZExpr* _cond;
	ZAst* _body;
};

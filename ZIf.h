#pragma once
#include "ZAst.h"
#include "ZExpr.h"
#include "ZVisitor.h"

class ZIf : public ZAst {
public:
	ZIf(ZExpr* cond, ZAst* body, ZAst* elseBody = nullptr) {
		_cond = cond;
		_body = body;
		_elseBody = elseBody;
        adopt(cond);
        adopt(body);
        if (elseBody)
            adopt(elseBody);
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
            _body = newChild;
            return;
        }

        if (_elseBody == oldChild) {
            _elseBody = newChild;
            return;
        }

        throw exception("wrong call to replaceChild in ZIf");
    }

	ZExpr* getCondition() {
		return _cond;
	}

	ZAst* getBody() {
		return _body;
	}

	ZAst* getElseBody() {
		return _elseBody;
	}

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "if\n";
		_cond->dump(stream, depth + 1);

		dumpTab(stream, depth);
		stream << "then\n";
		_body->dump(stream, depth + 1);
		if (_elseBody) {
			dumpTab(stream, depth);
			stream << "else\n";
			_elseBody->dump(stream, depth + 1);
		}
	}

private:
	ZExpr* _cond;
	ZAst* _body;
	ZAst* _elseBody;
};

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
	}

	void accept(ZVisitor* visitor) override {
		visitor->visit(this);
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

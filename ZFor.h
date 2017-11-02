#pragma once
#include "ZAst.h"
#include "ZExpr.h"

class ZFor : public ZAst {
public:

	ZFor(ZAst* body, ZAst* pre = nullptr, ZExpr* cond = nullptr, ZExpr* post = nullptr) {
		_body = body;
		_pre = pre;
		_cond = cond;
		_post = post;
	}

	ZAst* getBody() {
		return _body;
	}

	ZAst* getPre() {
		return _pre;
	}

	ZExpr* getCond() {
		return _cond;
	}

	ZExpr* getPost() {
		return _post;
	}

private:
	ZAst* _body;
	ZAst* _pre;
	ZExpr* _cond;
	ZExpr* _post;
};

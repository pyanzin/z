﻿#pragma once
#include "ZExpr.h"

class ZCast : public ZExpr {
public:
	ZCast(ZExpr* expr, ZType* targetType) {
		_expr = expr;
		setType(_expr->getType());
		_targetType = targetType;
		adopt(expr);
	}

	void replaceChild(ZAst* oldChild, ZAst* newChild) override {
		adopt(newChild);
		if (_expr == oldChild) {
			_expr = static_cast<ZExpr*>(newChild);
			return;
		}

		throw exception("wrong call to replaceChild in ZCast");
	}

	ZExpr* getExpr() {
		return _expr;
	}

	ZType* getTargetType() {
		return _targetType;
	}

private:
	ZExpr* _expr;
	ZType* _targetType;
};

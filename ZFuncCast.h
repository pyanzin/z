#include "ZExpr.h"
#include "ZFuncType.h"

class ZFuncCast : public ZExpr {
public:
    ZFuncCast(ZExpr* expr, ZFuncType* targetType, SymbolRef* ref) {
        _expr = expr;
        _targetType = targetType;
		_ref = ref;
    }

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);
        if (_expr == oldChild) {
            _expr = static_cast<ZExpr*>(newChild);
            return;
        }

        throw exception("wrong call to replaceChild in ZFuncCast");
    }

    ZExpr* getExpr() {
        return _expr;
    }

    ZFuncType* getTargetType() {
        return _targetType;
    }

	SymbolRef* getRef() {
		return _ref;
    }

private:
    ZExpr* _expr;
    ZFuncType* _targetType;
	SymbolRef* _ref;
};

#include "ZExpr.h"
#include "ZFuncType.h"

class ZFuncCast : public ZExpr {
public:
    ZFuncCast(ZExpr* expr, ZFuncType* targetType, SymbolRef* ref) {
        _expr = expr;
        _targetType = targetType;
        setRef(ref);
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

private:
    ZExpr* _expr;
    ZFuncType* _targetType;
};

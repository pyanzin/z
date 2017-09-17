#pragma once
#include "ZExpr.h"

class ZBooleanLit : public ZExpr {
public:
    ZBooleanLit(bool val) {
        _value = val;
        setType(Boolean);
    }

    bool getValue() {
        return _value;
    }
private:
    bool _value;
};

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

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << (_value ? "true" : "false") << "\n";
    }

private:
    bool _value;
};

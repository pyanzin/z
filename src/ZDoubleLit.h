#pragma once
#include "ZExpr.h"
#include "ZVisitor.h"

class ZDoubleLit : public ZExpr {
public:
    ZDoubleLit(double value) {
        _value = value;
        setType(Double);
    }

    virtual void accept(ZVisitor* visitor) {
        visitor->visit(this);
    }

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << "double(" << _value << ") : " << getType()->toString() << "\n";
    }

    double getValue() {
        return _value;
    }

private:
    double _value;
};

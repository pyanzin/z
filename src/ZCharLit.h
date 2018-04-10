#pragma once
#include "ZExpr.h"
#include "ZVisitor.h"

class ZCharLit : public ZExpr {
public:
    ZCharLit(char value) {
        _value = value;
        setType(Char);
    }

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    }

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);
        stream << "char(" << _value << ") : " << getType()->toString() << "\n";
    }

    char getValue() {
        return _value;
    }

private:
    char _value;
};

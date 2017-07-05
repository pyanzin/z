#ifndef ZINTCONST
#define ZINTCONST

#include <llvm/ADT/APInt.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include "ZExpr.h"
#include "Utils.h"

class ZIntLit : public ZExpr {
public:
    ZIntLit(int value) {
        _value = value;
		setType(Int);
    }

	virtual void accept(ZVisitor* visitor) {
		visitor->visit(this);
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "int(" << _value << ") : " << getType()->toString() << "\n";
	}

	int getValue() {
		return _value;
    }

private:
    int _value;
};



#endif
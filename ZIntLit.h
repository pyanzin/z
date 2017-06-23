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

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "int(" << _value << ") : " << toString(getType()) << "\n";
	}

private:
    int _value;
};



#endif
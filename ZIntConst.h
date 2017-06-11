#ifndef ZINTCONST
#define ZINTCONST

#include <llvm/ADT/APInt.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include "ZExpr.h"

class ZIntConst : public ZExpr {
public:
    ZIntConst(int value) {
        _value = value;
		setType(Int);
    }

private:
    int _value;
};



#endif
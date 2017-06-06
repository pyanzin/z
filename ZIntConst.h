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
    }

    llvm::Value* ZIntConst::codegen(llvm::Function * func) override {
        return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt::APInt(32, _value));
    }

private:
    int _value;
};



#endif
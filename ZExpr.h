#ifndef ZEXPR
#define ZEXPR

#include "llvm/IR/Type.h"
#include "ZFunc.h"
#include "ZAst.h"

class ZFunc;

class ZExpr : public ZAst {
public:
    virtual llvm::Value* codegen(llvm::Function * func) = 0;
};

#endif
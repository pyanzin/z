#ifndef ZBLOCK
#define ZBLOCK

#include "ZExpr.h"
#include <llvm/IR/Value.h>

class ZBlock : ZExpr {
public:
    ZBlock(std::vector<ZExpr*> expressions) {
        _expressions = expressions;
    }

    llvm::Value* codegen(llvm::Function * func) override = 0;

private:
    std::vector<ZExpr*> _expressions;
};

llvm::Value* ZBlock::codegen(llvm::Function * func) {
    auto expr = _expressions.begin();
    for (; expr < _expressions.end() - 1; ++expr) {
        (*expr)->codegen(func);
    }

    return (*expr)->codegen(func);
}

#endif
#ifndef ZVAR
#define ZVAR

#include <llvm/IR/Value.h>
#include "ZExpr.h"

class ZVar : public ZExpr {
public:
    ZVar(std::string name) {
        _name = name;
    }

    //llvm::Value* codegen(llvm::Function* func) override {
    //    for (auto& arg : func->args())
    //        if (arg.getName() == _name)
    //            return &(cast<Value>(arg));        
    //}

	std::string& getName() {
		return _name;
    }

private:
    std::string _name;
};

#endif
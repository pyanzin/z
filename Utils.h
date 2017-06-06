#ifndef ZUTILS
#define ZUTILS

#include "ZBasicTypes.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>

using namespace llvm;

IRBuilder<> builder(getGlobalContext());

enum BinOps {
    Sum,
    Sub,
    Mul,
    Div
};

Type* toLlvmType(BaseTypes ztype) {
    switch (ztype)
    {
    case Int:
        return IntegerType::get(getGlobalContext(), 32);
    case Double:
        break;
    case Boolean:
        return IntegerType::get(getGlobalContext(), 1);
    case String:
    default:
        return nullptr;
    }
}

#endif
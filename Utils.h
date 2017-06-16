#pragma once

#include "ZBasicTypes.h"
#include <llvm/IR/DerivedTypes.h>

using namespace llvm;

//IRBuilder<> builder(getGlobalContext());

enum BinOps {
    Sum,
    Sub,
    Mul,
    Div
};

Type* toLlvmType(BaseTypes ztype);
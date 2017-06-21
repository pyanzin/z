#pragma once

#include "ZBasicTypes.h"
#include <llvm/IR/DerivedTypes.h>
//#include "ZLexeme.h"

enum ZLexeme;

using namespace llvm;

//IRBuilder<> builder(getGlobalContext());

enum BinOps {
    Sum,
    Sub,
    Mul,
    Div
};

Type* toLlvmType(BaseTypes ztype);

std::string toString(ZLexeme lexeme);
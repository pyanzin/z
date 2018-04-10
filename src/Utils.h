#pragma once
#include <string>
#include <llvm/IR/LLVMContext.h>

enum ZLexeme;
enum BinOps;

enum BinOps {
    Sum,
    Sub,
    Mul,
    Div,
    Mod,
    Equal,
    NotEqual,
    Less,
    LessOrEqual,
    More,
    MoreOrEqual,
    BooleanOr,
    BooleanAnd,
    BooleanXor,
    BitwiseOr,
    BitwiseAnd
};

enum UnaryOps {
    Negation,
    BitwiseInvert,
    UnaryPlus,
    UnaryMinus,
    PreIncrement,
    PreDecrement,
    PostIncrement,
    PostDecrement
};

std::string toString(ZLexeme lexeme);

std::string toString(BinOps op);

std::string toString(UnaryOps op);

static llvm::LLVMContext ZContext;

inline llvm::LLVMContext& getLlvmContext() {
    return ZContext;
}

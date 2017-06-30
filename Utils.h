#ifndef UTILS_H
#define UTILS_H
#include <string>

enum ZLexeme;
enum BinOps;

//IRBuilder<> builder(getGlobalContext());

enum BinOps {
    Sum,
    Sub,
    Mul,
    Div
};

std::string toString(ZLexeme lexeme);

std::string toString(BinOps op);

#endif
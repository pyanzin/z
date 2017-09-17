﻿#pragma once
#include <string>

enum ZLexeme;
enum BinOps;

enum BinOps {
    Sum,
    Sub,
    Mul,
    Div,
	Equal,
	NotEqual,
	Less,
	LessOrEqual,
	More,
	MoreOrEqual
};

std::string toString(ZLexeme lexeme);

std::string toString(BinOps op);
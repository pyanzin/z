﻿#pragma once

enum ZLexeme {
    IDENT,
    DEF,
    STRUCT,
	EXTERN,
    VAR,
	RETURN,
	IF,
	ELSE,
	WHILE,
	FOR,
    STRING_LIT,
	INT_LIT,
	CHAR_LIT,
    BOOL_TRUE_LIT,
	BOOL_FALSE_LIT,
    OPEN_PAREN,
    CLOSE_PAREN,
    COLON,
    SEMICOLON,
    EQUAL,
    DOUBLE_EQUAL,
	NOT_EQUAL,
	LESS,
	LESS_OR_EQUAL,
	MORE,
	MORE_OR_EQUAL,
    OPEN_BRACE,
    CLOSE_BRACE,
	OPEN_BRACKET,
	CLOSE_BRACKET,
    FAT_ARROW,
    COMMA,
    DOT,
    PLUS,
    MINUS,
    ASTERISK,
	EXCLAM,
    SLASH,
    SIZEOF,
    AS,
    INPUT_END
};

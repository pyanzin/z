﻿#pragma once

enum ZLexeme {
    IDENT,
    DEF,
    STRUCT,
    CLASS,
    TRAIT,
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
	PIPE,
	AMPERSAND,
	CIRCUMFLEX,
	DOUBLE_PIPE,
	DOUBLE_AMPERSAND,
	TILDA,
    OPEN_BRACE,
    CLOSE_BRACE,
	OPEN_BRACKET,
	CLOSE_BRACKET,
    FAT_ARROW,
    COMMA,
    DOT,
    PLUS,
    MINUS,
	DOUBLE_PLUS,
	DOUBLE_MINUS,
    ASTERISK,
	EXCLAM,
    SLASH,
	PERCENT,
    SIZEOF,
    AS,
    INPUT_END
};

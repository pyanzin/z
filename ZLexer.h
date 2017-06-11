#pragma once
#include <string>
#include <map>
#include "ZLexeme.h"

enum ZLexeme;

class ZLexer
{
public:
    ZLexeme getNextToken();
    ZLexer(std::string src);
    ~ZLexer();

    int getPos() {
        return _pos;
    }

    void backtrackBy(int n) {
        _pos -= n;
    }

    void backtrackTo(int n) {
        _pos = n;
    }

    std::string* getValue() {
        return _value;
    }

    void reset() {
        _pos = 0;
    }

    std::string toString(ZLexeme lexeme) {
        switch (lexeme) {
            case IDENT:
                return "IDENT";
            case DEF:
                return "DEF";
            case VAR:
                return "VAR";
            case STRING_LIT:
                return "STRING_LIT";
            case OPEN_PAREN:
                return "OPEN_PAREN";
            case CLOSE_PAREN:
                return "CLOSE_PAREN";
            case COLON:
                return "COLON";
            case SEMICOLON:
                return "SEMICOLON";
            case EQUAL:
                return "EQUAL";
            case DOUBLE_EQUAL:
                return "DOUBLE_EQUAL";
            case OPEN_BRACE:
                return "OPEN_BRACE";
            case CLOSE_BRACE:
                return "CLOSE_BRACE";
            case FAT_ARROW:
                return "FAT_ARROW";
            case COMMA:
                return "COMMA";
            case INPUT_END:
                return "INPUT_END";
            
        }

    };
private:
    char getNextChar();
    std::string _src;
    std::string* _value;
    int _pos;
    std::map<std::string, ZLexeme> _keywords;

    void init() {
        _keywords.insert(std::make_pair("def", DEF));
        _keywords.insert(std::make_pair("var", VAR));
    }
};


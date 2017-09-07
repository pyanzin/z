#pragma once
#include <string>
#include <map>
#include "ZLexeme.h"

enum ZLexeme;
class SourceRange;

class ZLexer
{
public:
    ZLexeme getNextToken();
    ZLexer(std::string* src);
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

    SourceRange* beginRange();

    SourceRange* endRange(SourceRange* sr);

private:
    char getNextChar();
    std::string* _src;
    std::string* _value;
    int _pos;
    std::map<std::string, ZLexeme> _keywords;

    void init() {
        _keywords.insert(std::make_pair("def", DEF));
        _keywords.insert(std::make_pair("var", VAR));
		_keywords.insert(std::make_pair("return", RETURN));
		_keywords.insert(std::make_pair("while", WHILE));
		_keywords.insert(std::make_pair("if", IF));
		_keywords.insert(std::make_pair("else", ELSE));
		_keywords.insert(std::make_pair("extern", EXTERN));
    }
};


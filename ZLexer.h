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

    void backtrackBy(int n) {
        _pos -= n;
    }

    void backtrackTo(int n) {
        _pos = n;
    }

    std::string getValue() {
        return _value;
    }

    void reset() {
        _pos = 0;
    }

private:
    char getNextChar();
    std::string _src;
    std::string _value;
    int _pos;
    std::map<std::string, ZLexeme> _keywords;

    void init() {
        _keywords.insert(std::make_pair("def", DEF));
        _keywords.insert(std::make_pair("var", VAR));
    }
};


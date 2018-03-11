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
	char toEscape(char ch);
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

	void error(std::string text);

    void recoverToNextStatement();

    void recoverToTop();
	
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
        _keywords.insert(std::make_pair("true", BOOL_TRUE_LIT));
        _keywords.insert(std::make_pair("false", BOOL_FALSE_LIT));
        _keywords.insert(std::make_pair("for", FOR));
        _keywords.insert(std::make_pair("struct", STRUCT));
        _keywords.insert(std::make_pair("sizeof", SIZEOF));
        _keywords.insert(std::make_pair("as", AS));
        _keywords.insert(std::make_pair("class", CLASS));
        _keywords.insert(std::make_pair("trait", TRAIT));
        _keywords.insert(std::make_pair("new", NEW));        
    }
};


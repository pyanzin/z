#include "ZLexer.h"

ZLexer::ZLexer(std::string src) {
    _src = src;
    _pos = 0;
    init();
}

ZLexer::~ZLexer()
{
}

char ZLexer::getNextChar() {
    return _src[_pos++];
};

ZLexeme ZLexer::getNextToken() {
    _value = new std::string;
    char ch;
    while (isspace(ch = getNextChar()))
        ;

    switch (ch) {
    case '(':
        return OPEN_PAREN;
    case ')':
        return CLOSE_PAREN;
    case ':':
        return COLON;
    case ';':
        return SEMICOLON;
    case '{':
        return OPEN_BRACE;
    case '}':
        return CLOSE_BRACE;
    case '+':
        return PLUS;
    case '-':
        return MINUS;
    case '*':
        return ASTERISK;
    case '/':
        return SLASH;
    case '=':
        ch = getNextChar();
        if (ch == '=')
            return DOUBLE_EQUAL;
        if (ch == '>')
            return FAT_ARROW;
        backtrackBy(1);
        return EQUAL;
    case '"': {
        std::string* s = new std::string;
        while ((ch = getNextChar()) != '"')
            s += ch;
        _value = s;
        return STRING_LIT;
    }
    case '\0':
        return INPUT_END;
    }

    if (isalpha(ch)) {
        std::string id;
        id += ch;
        while (isalnum(ch = getNextChar()))
            id += ch;
        backtrackBy(1);
        bool isKeyword = _keywords.find(id) != _keywords.end();
        if (isKeyword)
            return _keywords[id];

        _value = new std::string(id);

        return IDENT;
    }

    throw std::string("Unable to recognize a token starting with: " + ch);
}
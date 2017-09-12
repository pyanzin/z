#include "ZLexer.h"
#include "SourceRange.h"

ZLexer::ZLexer(std::string* src) {
    _src = src;
    _pos = 0;
    init();
}

ZLexer::~ZLexer()
{
}

SourceRange* ZLexer::beginRange() {
    return new SourceRange(_src, _pos);
}

SourceRange* ZLexer::endRange(SourceRange* sr) {
    sr->setEnd(_pos);
    return sr;
}

char ZLexer::getNextChar() {
    return (*_src)[_pos++];
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
	case ',':
		return COMMA;
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
		if (getNextChar() == '/') {
			while (getNextChar() != '\n')
				;
			return getNextToken();
		} else {
			backtrackBy(1);
			return SLASH;
	    }
	case '<':
		ch = getNextChar();
		if (ch == '=')
			return LESS_OR_EQUAL;
		backtrackBy(1);
		return LESS;
	case '>':
		ch = getNextChar();
		if (ch == '=')
			return MORE_OR_EQUAL;
		backtrackBy(1);
		return MORE;
    case '=':
        ch = getNextChar();
        if (ch == '=')
            return DOUBLE_EQUAL;
        if (ch == '>')
            return FAT_ARROW;
        backtrackBy(1);
        return EQUAL;
	case '!':
		ch = getNextChar();
		if (ch == '=')
			return NOT_EQUAL;
		backtrackBy(1);
		return EXCLAM;
	case '"': {
		std::string* s = new std::string;
		while ((ch = getNextChar()) != '"') {
			if (ch == '\n')
				error("Incorrect string literal at " + _pos);			
			if (ch == '\\')
				ch = toEscape(getNextChar());			
			(*s) += ch;
		}
		_value = s;
		return STRING_LIT;
	}
	case '\'': {
		std::string* s = new std::string;
		ch = getNextChar();
		if (ch == '\\')
			ch = toEscape(ch);
		(*s) += ch;
		_value = s;
		if (getNextChar() != '\'')
			error("Incorrect char literal at " + _pos);
		
		return CHAR_LIT;
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

	if (isdigit(ch)) {
		std::string value;
		value += ch;
		while (isalnum(ch = getNextChar()))
			value += ch;
		backtrackBy(1);
		_value = new std::string(value);
		return INT_LIT;
	}

    error("Unable to recognize a token starting with: " + ch);
}

char ZLexer::toEscape(char ch) {
	switch (ch) {
	case 'n':
		return '\n';
	case 't':
		return '\t';
	case '\\':
		return '\\';
	case '\'':
		return '\'';
	case '\"':
		return '\"';
	case 'r':
		return '\r';
	case 'v':
		return '\v';
	case '0':
		return '\0';	
	}

	error("Unknown escape sequence: \\" + ch);
}

void ZLexer::error(std::string text) {
	throw new exception(text.c_str());
};
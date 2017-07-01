#include "ZParser.h"
#include "ZArg.h"
#include "ZStringLit.h"

ZFunc* ZParser::parseFunc() {
	if (!consume(DEF))
		return nullptr;


	std::string* name = reqVal(IDENT);
	reqConsume(OPEN_PAREN);

	std::vector<ZArg*>* args = new std::vector<ZArg*>();
	while (!consume(CLOSE_PAREN)) {
		args->push_back(parseArg());
		consume(COMMA);
	}

	reqConsume(COLON);

	ZType* retType = parseType();

	reqConsume(EQUAL);

	ZBlock* body = parseBlock();

	return new ZFunc(name, retType, *args, body);
}

ZArg* ZParser::parseArg() {
	std::string* name = reqVal(IDENT);
	reqConsume(COLON);
	ZType* type = parseType();

	return new ZArg(type, name);
}

ZBlock* ZParser::parseBlock() {
	std::vector<ZAst*>* stmts = new std::vector<ZAst*>;

	reqConsume(OPEN_BRACE);

	while (!consume(CLOSE_BRACE)) {
		ZAst* stmt = parseStatement();
		reqConsume(SEMICOLON);
		stmts->push_back(stmt);
	}

	return new ZBlock(stmts);
}

ZAst* ZParser::parseStatement() {
	int pos = _lexer.getPos();
	if (consume(VAR)) {
		_lexer.backtrackTo(pos);
		return parseVarDef();
	}

	return parseExpr();
}

ZVarDef* ZParser::parseVarDef() {
	reqConsume(VAR);
	std::string* name = reqVal(IDENT);
	reqConsume(COLON);
	ZType* type = parseType();
    
    ZExpr* initExpr = nullptr;

    if (consume(EQUAL))
        initExpr = parseExpr();

	return new ZVarDef(*name, type, initExpr);
}

ZExpr* ZParser::parseAssign() {
	int pos = _lexer.getPos();

	ZExpr* left = parseId();

	if (left && consume(EQUAL))
		return new ZAssign(left, parseAssign());

	_lexer.backtrackTo(pos);
	return parseBinOp();
}

ZExpr* ZParser::parseBinOp() {
	int pos = _lexer.getPos();
	ZExpr* left = parseId();

	if (consume(PLUS))
		return new ZBinOp(left, parseExpr(), BinOps::Sum);
	else if (consume(MINUS))
		return new ZBinOp(left, parseExpr(), BinOps::Sub);
	else if (consume(ASTERISK))
		return new ZBinOp(left, parseExpr(), BinOps::Mul);
	else if (consume(SLASH))
		return new ZBinOp(left, parseExpr(), BinOps::Div);

	_lexer.backtrackTo(pos);
	return parseCall();
}

ZExpr* ZParser::parseCall() {
	int pos = _lexer.getPos();
	// TODO: use expr here

	ZExpr* callee = parseId();
	if (!consume(OPEN_PAREN)) {
		_lexer.backtrackTo(pos);
		return parseId();
	}

	std::vector<ZExpr*>* args = new std::vector<ZExpr*>();
	while (!consume(CLOSE_PAREN)) {
		ZExpr* arg = parseExpr();
		consume(COMMA);
		args->push_back(arg);
	}

	return new ZCall(callee, *args);
}

ZExpr* ZParser::parseId() {
	std::string* name = val(IDENT);
	if (!name)
		return parseString();

	return new ZId(*name);
}

ZExpr* ZParser::parseString() {
	std::string* value = val(STRING_LIT);
	if (!value)
		return parseNumber();

	return new ZStringLit(*value);
}

ZExpr* ZParser::parseNumber() {
	if (consume(OPEN_PAREN)) {
		ZExpr* expr = parseExpr();
		reqConsume(CLOSE_PAREN);
		return expr;
	}

	std::string* value = val(INT_LIT);
	if (!value)
		return nullptr;

	return new ZIntLit(std::stoi((*value).c_str()));
}

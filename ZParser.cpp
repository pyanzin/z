#include "ZParser.h"
#include "ZArg.h"
#include "ZStringLit.h"
#include "ZReturn.h"
#include "ZIf.h"
#include "ZWhile.h"

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

    std::vector<ZType*>* argTypes = new std::vector<ZType*>();
	for (ZArg* arg : *args) {
		argTypes->push_back(arg->getType());
		_symTable.add(arg->getType(), arg->getName());
	}

    ZType* funcType = new ZFuncType(retType, *argTypes);

    _symTable.add(funcType, name);

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
    _symTable.enter();
	std::vector<ZAst*>* stmts = new std::vector<ZAst*>;

	reqConsume(OPEN_BRACE);

	while (!consume(CLOSE_BRACE)) {
		ZAst* stmt = parseStatement();
		reqConsume(SEMICOLON);
		stmts->push_back(stmt);
	}

    _symTable.exit();

	return new ZBlock(stmts);
}

ZAst* ZParser::parseReturn() {
	reqConsume(RETURN);
	return new ZReturn(parseExpr());
}

ZAst* ZParser::parseIf() {
	reqConsume(IF);
	reqConsume(OPEN_PAREN);
	ZExpr* cond = parseExpr();
	reqConsume(CLOSE_PAREN);

	ZAst* body = parseBlock();
	ZAst* elseBody = nullptr;
	if (consume(ELSE))
		elseBody = parseBlock();

	return new ZIf(cond, body, elseBody);	
}

ZAst* ZParser::parseWhile() {
	reqConsume(WHILE);
	reqConsume(OPEN_PAREN);
	ZExpr* cond = parseExpr();
	reqConsume(CLOSE_PAREN);

	ZAst* body = parseBlock();

	return new ZWhile(cond, body);
}

ZAst* ZParser::parseStatement() {
	int pos = _lexer.getPos();
	if (consume(VAR)) {
		_lexer.backtrackTo(pos);
		return parseVarDef();
	} else if (consume(RETURN)) {
		_lexer.backtrackTo(pos);
		return parseReturn();
	} else if (consume(IF)) {
		_lexer.backtrackTo(pos);
		return parseIf();
	}
	else if (consume(WHILE)) {
		_lexer.backtrackTo(pos);
		return parseWhile();
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

    _symTable.add(type, name);

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
	else if (consume(DOUBLE_EQUAL))
		return new ZBinOp(left, parseExpr(), BinOps::Equal);
	else if (consume(LESS))
		return new ZBinOp(left, parseExpr(), BinOps::Less);
	else if (consume(LESS_OR_EQUAL))
		return new ZBinOp(left, parseExpr(), BinOps::LessOrEqual);
	else if (consume(MORE))
		return new ZBinOp(left, parseExpr(), BinOps::More);
	else if (consume(MORE_OR_EQUAL))
		return new ZBinOp(left, parseExpr(), BinOps::MoreOrEqual);

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
    SymbolRef* ref = _symTable.makeRef();
	return new ZId(*name, *ref);
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

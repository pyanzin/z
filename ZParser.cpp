#include "ZParser.h"
#include "ZArg.h"
#include "ZStringLit.h"
#include "ZReturn.h"
#include "ZIf.h"
#include "ZWhile.h"
#include "ZIntLit.h"
#include "ZCharLit.h"
#include "ZLexeme.h"
#include "ZAssign.h"
#include "ZCall.h"
#include "ZId.h"
#include "SourceRange.h"
#include "ZLexeme.h"
#include "ZBinOp.h"

ZParser::ZParser(ZLexer& lexer, SymbolTable& symTable): _lexer(lexer), _symTable(symTable) {
    _types["Int"] = Int;
	_types["Char"] = Char;
    _types["String"] = String;
    _types["Boolean"] = Boolean;
    _types["Double"] = Double;
    _types["None"] = Void;

	_ops[PLUS] = Sum;
	_ops[MINUS] = Sub;
	_ops[ASTERISK] = Mul;
	_ops[SLASH] = Div;
	_ops[DOUBLE_EQUAL] = Equal;
	_ops[NOT_EQUAL] = NotEqual;
	_ops[LESS] = Less;
	_ops[LESS_OR_EQUAL] = LessOrEqual;
	_ops[MORE] = More;
	_ops[MORE_OR_EQUAL] = MoreOrEqual;
}

ZModule* ZParser::parseModule() {
    auto modName = new std::string("test"); // TODO: user real mod name
    ZModule* module = new ZModule(*modName);
    ZFunc* func;
    while (func = parseFunc())
        module->addFunction(func);

    return module;
}

ZFunc* ZParser::parseFunc() {
	auto sr = beginRange();

	bool isExtern = consume(EXTERN);

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

	if (!isExtern)
		reqConsume(EQUAL);	

	_symTable.enter();

    std::vector<ZType*>* argTypes = new std::vector<ZType*>();
	for (ZArg* arg : *args) {
		argTypes->push_back(arg->getType());
		_symTable.add(arg->getType(), arg->getName());
	}

    ZType* funcType = new ZFuncType(retType, *argTypes);

	ZBlock* body = nullptr;
	if (!isExtern)
		body = parseBlock();

	_symTable.exit();

	_symTable.add(funcType, name);
	
    auto zfunc = new ZFunc(name, retType, *args, body, isExtern);
    zfunc->setType(funcType);

	zfunc->withSourceRange(endRange(sr));

	return zfunc;
}

ZArg* ZParser::parseArg() {
	std::string* name = reqVal(IDENT);
	reqConsume(COLON);
	ZType* type = parseType();

	return new ZArg(type, name);
}

ZType* ZParser::parseType() {
	if (consume(OPEN_PAREN)) {
		std::vector<ZType*>* argTypes = new std::vector<ZType*>;
		while (!consume(CLOSE_PAREN)) {
			ZType* argType = parseType();
			argTypes->push_back(argType);
			consume(COMMA);
		}

		reqConsume(FAT_ARROW);
		ZType* retType = parseType();

		return new ZFuncType(retType, *argTypes);
	}
	return _types[*reqVal(IDENT)];
}

ZBlock* ZParser::parseBlock() {
	auto sr = beginRange();

	std::vector<ZAst*>* stmts = new std::vector<ZAst*>;

	reqConsume(OPEN_BRACE);

	while (!consume(CLOSE_BRACE)) {
		ZAst* stmt = parseStatement();
		reqConsume(SEMICOLON);
		stmts->push_back(stmt);
	}

	auto zblock = new ZBlock(stmts);
	zblock->withSourceRange(endRange(sr));

	return zblock;
}

ZAst* ZParser::parseReturn() {
	auto sr = beginRange();

	reqConsume(RETURN);
	return (new ZReturn(parseExpr()))->withSourceRange(endRange(sr));
}

ZAst* ZParser::parseIf() {
	auto sr = beginRange();

	reqConsume(IF);
	reqConsume(OPEN_PAREN);
	ZExpr* cond = parseExpr();
	reqConsume(CLOSE_PAREN);

	ZAst* body = parseBlock();
	ZAst* elseBody = nullptr;
	if (consume(ELSE))
		elseBody = parseBlock();

	return (new ZIf(cond, body, elseBody))->withSourceRange(endRange(sr));	
}

ZAst* ZParser::parseWhile() {
	auto sr = beginRange();

	reqConsume(WHILE);
	reqConsume(OPEN_PAREN);
	ZExpr* cond = parseExpr();
	reqConsume(CLOSE_PAREN);

	ZAst* body = parseBlock();

	sr = endRange(sr);

	return (new ZWhile(cond, body))->withSourceRange(sr);
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
	auto sr = beginRange();

	reqConsume(VAR);
	std::string* name = reqVal(IDENT);
	ZType* type = consume(COLON) ? parseType() : Unknown;
    
    ZExpr* initExpr = nullptr;

    if (consume(EQUAL))
        initExpr = parseExpr();

    auto ref = _symTable.add(type, name);

	ZVarDef* zvardef = new ZVarDef(*name, *ref, type, initExpr);

	zvardef->withSourceRange(endRange(sr));

	return zvardef;
}

ZExpr* ZParser::parseAssign() {
	auto sr = beginRange();

	int pos = _lexer.getPos();

	ZExpr* left = parseId();

	if (left && consume(EQUAL)) {
		auto right = parseAssign();
		auto zassign = new ZAssign(left, right);
		zassign->withSourceRange(endRange(sr));
		return zassign;
	}

	_lexer.backtrackTo(pos);
	return parseBinOp();
}

ZExpr* ZParser::parseBinOp() {
	int pos = _lexer.getPos();

	auto sr = beginRange();

	ZExpr* left = parseCall();
	auto next = _lexer.getNextToken();

	if (_ops.find(next) == _ops.end()) {
		_lexer.backtrackTo(pos);
		return parseCall();
	}

	auto right = parseExpr();

	auto zbinop = new ZBinOp(left, right, _ops[next]);

	zbinop->withSourceRange(endRange(sr));

	return zbinop;
}

ZExpr* ZParser::parseCall() {
	int pos = _lexer.getPos();
	// TODO: use expr here

	auto sr = beginRange();

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

	ZCall* zcall = new ZCall(callee, *args);

	zcall->withSourceRange(endRange(sr));

	return zcall;
}

ZExpr* ZParser::parseId() {
	auto sr = beginRange();

    std::string* name = val(IDENT);
	if (!name)
		return parseString();
    SymbolRef* ref = _symTable.makeRef();

	ZExpr* zid = new ZId(*name, *ref);

	zid->withSourceRange(endRange(sr));

	return zid;
}

ZExpr* ZParser::parseString() {
	auto sr = beginRange();

	std::string* value = val(STRING_LIT);
	if (!value)
		return parseChar();

	auto zstr = new ZStringLit(*value);
	zstr->withSourceRange(endRange(sr));

	return zstr;
}

ZExpr* ZParser::parseChar() {
	auto sr = beginRange();

	if (!isNext(CHAR_LIT))
		return parseNumber();
	

	std::string* value = val(CHAR_LIT);
	if (!value)
		return nullptr;

	auto zcharlit = new ZCharLit((*value)[0]);

	zcharlit->withSourceRange(endRange(sr));

	return zcharlit;
}

ZExpr* ZParser::parseNumber() {
	auto sr = beginRange();

	if (consume(OPEN_PAREN)) {
		ZExpr* expr = parseExpr();
		reqConsume(CLOSE_PAREN);
		return expr;
	}

	std::string* value = val(INT_LIT);
	if (!value)
		return nullptr;

	auto zintlit = new ZIntLit(std::stoi((*value).c_str()));

	zintlit->withSourceRange(endRange(sr));

	return zintlit;
}

std::string* ZParser::val(::ZLexeme lexeme) {
    if (consume(lexeme))
        return _lexer.getValue();
    else
        return nullptr;
}

bool ZParser::consume(::ZLexeme lexeme) {
    int pos = _lexer.getPos();
        
    if (_lexer.getNextToken() == lexeme)
        return true;
    else {
        _lexer.backtrackTo(pos);
        return false;
    }
}

bool ZParser::isNext(::ZLexeme lexeme) {
	int pos = _lexer.getPos();

	ZLexeme next = _lexer.getNextToken();

	_lexer.backtrackTo(pos);
	if (next == lexeme)
		return true;
	else
		return false;	
}

void ZParser::reqConsume(::ZLexeme lexeme) {
    if (!consume(lexeme))
        error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()));
}

std::string* ZParser::reqVal(::ZLexeme lexeme) {
    std::string* value = val(lexeme);
    if (value)
        return value;
    else
        error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()));

}

SourceRange* ZParser::beginRange() {
    return _lexer.beginRange();
};

SourceRange* ZParser::endRange(SourceRange* sr) {
    return _lexer.endRange(sr);
};
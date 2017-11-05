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
#include "ZBinOp.h"
#include "ZBooleanLit.h"
#include "ZNop.h"
#include "ZArrayType.h"
#include "ZGenericParam.h"
#include "ZFuncType.h"
#include "ZLambda.h"
#include "ZFor.h"

ZParser::ZParser(ZLexer& lexer, SymbolTable& symTable): _lexer(lexer), _symTable(symTable) {
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

	_symTable.addType(Int);
	_symTable.addType(Char);
	_symTable.addType(String);
	_symTable.addType(Boolean);
	_symTable.addType(Double);
	_symTable.addType(Void);
	_symTable.addType(Any);
	_symTable.addType(new ZArrayType(nullptr));
	_symTable.addType(Wildcard);
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

	_symTable.enter();

	std::vector<ZGenericParam*>* typeParams = new std::vector<ZGenericParam*>;
	if (consume(OPEN_BRACKET)) {
		while (!consume(CLOSE_BRACKET)) {
			ZGenericParam* typeParam = new ZGenericParam(*reqVal(IDENT));
			typeParams->push_back(typeParam);
			_symTable.addType(typeParam);
			consume(COMMA);
		}
	}

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

    std::vector<ZType*>* argTypes = new std::vector<ZType*>();
	for (ZArg* arg : *args) {
		argTypes->push_back(arg->getType());
		_symTable.addSymbol(arg->getType(), arg->getName());
	}

    ZType* funcType = new ZFuncType(retType, *argTypes, typeParams);

	ZBlock* body = nullptr;
	if (!isExtern)
		body = parseBlock();

	_symTable.exit();

	_symTable.addSymbol(funcType, name);
	
    auto zfunc = new ZFunc(name, retType, *args, *typeParams, body, isExtern);
    zfunc->setType(funcType);

	zfunc->withSourceRange(endRange(sr));

	return zfunc;
}

ZExpr* ZParser::parseLambda() {
	int pos = _lexer.getPos();

	auto sr = beginRange();	

	if (!consume(OPEN_PAREN)) {
		_lexer.backtrackTo(pos);
		return parseNumber();
	}

	_symTable.enter();

	std::vector<ZArg*>* args = new std::vector<ZArg*>();
	while (!consume(CLOSE_PAREN)) {
		ZArg* arg = parseArg();
		if (!arg) {
			_symTable.exit();
			_lexer.backtrackTo(pos);
			return parseNumber();
		}
		args->push_back(arg);
		consume(COMMA);
	}

	std::vector<ZType*>* argTypes = new std::vector<ZType*>();
	for (ZArg* arg : *args) {
		argTypes->push_back(arg->getType());
		_symTable.addSymbol(arg->getType(), arg->getName());
	}

	reqConsume(COLON);

	ZType* retType = parseType();

	reqConsume(FAT_ARROW);

	ZExpr* expr = parseExpr();

	_symTable.exit();
    
	ZFuncType* lambdaType = new ZFuncType(retType, *argTypes);
    auto lambda = new ZLambda(args, expr);
	lambda->withSourceRange(endRange(sr));
	return lambda;
}

ZArg* ZParser::parseArg() {
	std::string* name = val(IDENT);
	if (!name)
		return nullptr;
	
	if (!consume(COLON))
		return nullptr;

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

	auto type = _symTable.makeRef()->findTypeDef(*reqVal(IDENT));

	if (dynamic_cast<ZArrayType*>(type)) {
		reqConsume(OPEN_BRACKET);
		auto elemType = parseType();
		reqConsume(CLOSE_BRACKET);
		return new ZArrayType(elemType);
	}

	return type;
}

ZBlock* ZParser::parseBlock() {
	auto sr = beginRange();

	std::vector<ZAst*>* stmts = new std::vector<ZAst*>;

	reqConsume(OPEN_BRACE);

	while (!consume(CLOSE_BRACE)) {
		ZAst* stmt = parseBlockOrStatement();
		stmts->push_back(stmt);
	}

	auto zblock = new ZBlock(stmts);
	zblock->withSourceRange(endRange(sr));

	return zblock;
}

ZAst* ZParser::parseBlockOrStatement() {
	if (consume(SEMICOLON))
		return new ZNop();

	if (isNext(OPEN_BRACE))
		return parseBlock();

	return parseStatement();
}

ZAst* ZParser::parseStatement() {
	if (isNext(IF))
		return parseIf();
	if (isNext(WHILE))
		return parseWhile();
	if (isNext(FOR))
		return parseFor();

	ZAst* result;
	if (isNext(VAR))
		result = parseVarDef();
	else if (isNext(RETURN))
		result = parseReturn();
	else
		result = parseExpr();

	reqConsume(SEMICOLON);

	return result;
}

ZAst* ZParser::parseReturn() {
	auto sr = beginRange();

	reqConsume(RETURN);

    ZExpr* expr = parseExpr();

    auto ret = (new ZReturn(expr))->withSourceRange(endRange(sr));

	return ret;
}

ZAst* ZParser::parseIf() {
	auto sr = beginRange();

	reqConsume(IF);
	reqConsume(OPEN_PAREN);
	ZExpr* cond = parseExpr();
	reqConsume(CLOSE_PAREN);

	ZAst* body = parseBlockOrStatement();
	ZAst* elseBody = nullptr;
	if (consume(ELSE))
		elseBody = parseBlockOrStatement();

	return (new ZIf(cond, body, elseBody))->withSourceRange(endRange(sr));	
}

ZAst* ZParser::parseWhile() {
	auto sr = beginRange();

	reqConsume(WHILE);
	reqConsume(OPEN_PAREN);
	ZExpr* cond = parseExpr();
	reqConsume(CLOSE_PAREN);

	ZAst* body = parseBlockOrStatement();

	sr = endRange(sr);

	return (new ZWhile(cond, body))->withSourceRange(sr);
}

ZAst* ZParser::parseFor() {
	auto sr = beginRange();

	_symTable.enter();

	reqConsume(FOR);
	reqConsume(OPEN_PAREN);
	ZAst* pre = parseBlockOrStatement();

	ZExpr* cond;
	if (consume(SEMICOLON))
		cond = new ZBooleanLit(true);
	else {
		cond = parseExpr();
		reqConsume(SEMICOLON);
	}

	ZExpr* post;
	if (isNext(CLOSE_PAREN))
		post = nullptr;
	else
		post = parseExpr();	

	reqConsume(CLOSE_PAREN);

	ZAst* body = parseBlockOrStatement();

	_symTable.exit();

	sr = endRange(sr);

	return (new ZFor(body, pre, cond, post))->withSourceRange(sr);
}

ZVarDef* ZParser::parseVarDef() {
	auto sr = beginRange();

	reqConsume(VAR);
	std::string* name = reqVal(IDENT);
	ZType* type = consume(COLON) ? parseType() : Unknown;
    
    ZExpr* initExpr = nullptr;

    if (consume(EQUAL))
        initExpr = parseExpr();

    auto ref = _symTable.addSymbol(type, name);

	ZVarDef* zvardef = new ZVarDef(*name, *ref, type, initExpr);

	zvardef->withSourceRange(endRange(sr));

	return zvardef;
}

ZExpr* ZParser::parseAssign() {
	auto sr = beginRange();

	int pos = _lexer.getPos();

	ZExpr* left = parseCall();

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

	auto zbinop = new ZBinOp(left, right, _ops[next], _symTable.makeRef());

	zbinop->withSourceRange(endRange(sr));

	return zbinop;
}

ZExpr* ZParser::parseCall() {
	int pos = _lexer.getPos();
	// TODO: use expr here

	auto sr = beginRange();

	ZExpr* callee = parseId();

	_symTable.enter();

	vector<ZType*>* typeParams = new vector<ZType*>;
	if (consume(OPEN_BRACKET)) {
		while (!consume(CLOSE_BRACKET)) {
			ZType* typeParam = parseType();
			typeParams->push_back(typeParam);
			consume(COMMA);
		}
	}

	if (!consume(OPEN_PAREN)) {
		_lexer.backtrackTo(pos);
		_symTable.exit();
		return parseId();
	}

	std::vector<ZExpr*>* args = new std::vector<ZExpr*>();
	while (!consume(CLOSE_PAREN)) {
		ZExpr* arg = parseExpr();
		consume(COMMA);
		args->push_back(arg);
	}

	SymbolRef* ref = _symTable.makeRef();

	_symTable.exit();

	ZCall* zcall = new ZCall(callee, *args, typeParams, ref);

	zcall->withSourceRange(endRange(sr));

	return zcall;
}

ZExpr* ZParser::parseId() {
	auto sr = beginRange();

    std::string* name = val(IDENT);
	if (!name)
		return parseString();
    SymbolRef* ref = _symTable.makeRef();

	ZExpr* zid = new ZId(*name, ref);

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
		return parseLambda();
	

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
        return parseBoolean();

	auto zintlit = new ZIntLit(std::stoi((*value).c_str()));

	zintlit->withSourceRange(endRange(sr));

	return zintlit;
}

ZExpr* ZParser::parseBoolean() {
    auto value = val(BOOL_LIT);
    if (!value)
        return nullptr;

    if (string("true") == *value)
        return new ZBooleanLit(true);
    else
        return new ZBooleanLit(false);
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
	if (!consume(lexeme)) {
		auto pos = _lexer.beginRange()->getPosition();
		error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()) + " at " + pos);
	}
}

std::string* ZParser::reqVal(::ZLexeme lexeme) {
    std::string* value = val(lexeme);
    if (value)
        return value;
	else {
		auto pos = _lexer.beginRange()->getPosition();
		error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()) + " at " + pos);
	}

}

SourceRange* ZParser::beginRange() {
    return _lexer.beginRange();
};

SourceRange* ZParser::endRange(SourceRange* sr) {
    return _lexer.endRange(sr);
};
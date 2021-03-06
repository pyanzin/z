﻿#include "ZParser.h"
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
#include "ZClassType.h"
#include "ZLambda.h"
#include "ZFor.h"
#include "ZStructType.h"
#include "ZSelector.h"
#include "ZCast.h"
#include "ZSizeOf.h"
#include "RecoveryException.h"
#include "ZClassDef.h"
#include "ZDoubleLit.h"

ZParser::ZParser(ZLexer& lexer, SymbolTable& symTable): _lexer(lexer), _symTable(symTable) {
    _binOps[PLUS] = Sum;
    _binOps[MINUS] = Sub;
    _binOps[ASTERISK] = Mul;
    _binOps[SLASH] = Div;
    _binOps[PERCENT] = Mod;
    _binOps[DOUBLE_EQUAL] = Equal;
    _binOps[NOT_EQUAL] = NotEqual;
    _binOps[LESS] = Less;
    _binOps[LESS_OR_EQUAL] = LessOrEqual;
    _binOps[MORE] = More;
    _binOps[MORE_OR_EQUAL] = MoreOrEqual;
    _binOps[DOUBLE_AMPERSAND] = BooleanAnd;
    _binOps[DOUBLE_PIPE] = BooleanOr;
    _binOps[CIRCUMFLEX] = BooleanXor;
    _binOps[AMPERSAND] = BitwiseAnd;
    _binOps[PIPE] = BitwiseOr;

    _binOpPriority[Mul] = 100;
    _binOpPriority[Div] = 100;
    _binOpPriority[Mod] = 100;
    _binOpPriority[Sum] = 90;
    _binOpPriority[Sub] = 90;
    _binOpPriority[Less] = 80;
    _binOpPriority[LessOrEqual] = 80;
    _binOpPriority[More] = 80;
    _binOpPriority[MoreOrEqual] = 80;
    _binOpPriority[Equal] = 70;
    _binOpPriority[NotEqual] = 70;
    _binOpPriority[BitwiseAnd] = 60;
    _binOpPriority[BooleanXor] = 50;
    _binOpPriority[BitwiseOr] = 40;
    _binOpPriority[BooleanAnd] = 30;
    _binOpPriority[BooleanOr] = 20;

    _unaryOps[EXCLAM] = Negation;
    _unaryOps[TILDA] = BitwiseInvert;
    _unaryOps[PLUS] = UnaryPlus;
    _unaryOps[MINUS] = UnaryMinus;
    _unaryOps[DOUBLE_PLUS] = PreIncrement;
    _unaryOps[DOUBLE_MINUS] = PreDecrement;

    _symTable.addType(Int);
    _symTable.addType(Char);
    _symTable.addType(String);
    _symTable.addType(Boolean);
    _symTable.addType(Double);
    _symTable.addType(Void);
    _symTable.addType(Any);
    _symTable.addType(Ref);
    _symTable.addType(new ZArrayType(Unknown));
    _symTable.addType(Wildcard);
}

ZModule* ZParser::parseModule() {
    auto modName = new std::string("test"); // TODO: user real mod name
    _module = new ZModule(*modName);

    while (!isNext(INPUT_END)) {
        try {
            if (isNext(DEF) || isNext(EXTERN))
                _module->addFunction(parseFunc());
            else if (isNext(STRUCT))
                parseStruct();
            else if (isNext(CLASS))
                parseClass();
            else
                error("Expected function, class or stuct definition, but found: " + toString(_lexer.getNextToken()),
                      _lexer.beginRange());
        }
        catch (RecoveryException error) {
            _lexer.recoverToTop();
        }
    }

    return _module;
}

void ZParser::parseStruct() {
    reqConsume(STRUCT);

    std::string* name = reqVal(IDENT);
    reqConsume(OPEN_PAREN);

    _symTable.enter();

    std::vector<ZArg*>* members = new std::vector<ZArg*>();
    while (!consume(CLOSE_PAREN)) {
        auto member = parseFullArg();
        members->push_back(member);

        _symTable.addSymbol(member->getType(), member->getName(), StackVar);

        consume(COMMA);
    }

    auto structType = new ZStructType(name, members);

    auto varName = new string("__constructee");

    auto objDef = new ZVarDef(*varName, *_symTable.makeRef(), structType, new ZCast(
                                  new ZCall(new ZId(*new string("allocate"), _symTable.makeRef()),
                                            *new vector<ZExpr*>() = {new ZSizeOf(structType, true)},
                                            new std::vector<ZType*>, _symTable.makeRef()), structType,
                                  _symTable.makeRef()));

    _symTable.addSymbol(structType, varName, SymbolType::StackVar);

    auto stmts = new std::vector<ZAst*>();

    stmts->push_back(objDef);

    for (auto member : *members) {
        auto init = new ZAssign(
            new ZSelector(new ZId(*varName, _symTable.makeRef()), member->getName(), _symTable.makeRef()),
            new ZId(*member->getName(), _symTable.makeRef()),
            _symTable.makeRef());
        stmts->push_back(init);
    }

    stmts->push_back(new ZReturn(new ZId(*varName, _symTable.makeRef())));

    SymbolRef* scopeRef = _symTable.makeRef();

    _symTable.exit();

    auto ctor = new ZFunc(name, structType, *members, *new vector<ZGenericParam*>(), new ZBlock(stmts), scopeRef);

    _module->addFunction(ctor);
    _symTable.addSymbol(ctor->getType(), name, SymbolType::GlobalFunc);
    _symTable.addType(structType);
}

void ZParser::parseClass() {
    reqConsume(CLASS);
    string* name = reqVal(IDENT);

    _symTable.enter(false);

    std::vector<ZGenericParam*>* typeParams = new std::vector<ZGenericParam*>;
    if (consume(OPEN_BRACKET)) {
        while (!consume(CLOSE_BRACKET)) {
            ZGenericParam* typeParam = new ZGenericParam(*reqVal(IDENT));
            typeParams->push_back(typeParam);
            _symTable.addType(typeParam);
            consume(COMMA);
        }
    }

    // todo: inheritance stuff here

    auto fields = new std::vector<ZArg*>();
    auto methods = new std::vector<ZFunc*>();

    if (consume(OPEN_BRACE)) {
        while (!consume(CLOSE_BRACE)) {
            if (isNext(VAR)) {
                auto field = parseField();
                fields->push_back(field);
                _symTable.addSymbol(field->getType(), field->getName(), Field);
            }
            else if (isNext(DEF)) {
                auto method = parseFunc(true);
                methods->push_back(method);
            }
        }
    }

    SymbolRef* classScope = _symTable.makeRef();

    _symTable.exit();

    auto classType = new ZClassType(name, fields, classScope, typeParams);

    _symTable.addType(classType);
    // todo: make constructors public

    _module->addClass(new ZClassDef(*name, classType, *methods));
}

ZArg* ZParser::parseField() {
    auto sr = beginRange();

    reqConsume(VAR);
    std::string* name = reqVal(IDENT);
    reqConsume(COLON);
    ZType* type = parseType();
    reqConsume(SEMICOLON);

    ZArg* result = new ZArg(type, name, _symTable.makeRef());
    result->withSourceRange(endRange(sr));

    return result;
}

ZFunc* ZParser::parseFunc(bool isMethod) {
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
        args->push_back(parseFullArg());
        consume(COMMA);
    }

    reqConsume(COLON);

    ZType* retType = parseType();

    if (!isExtern)
        reqConsume(EQUAL);

    std::vector<ZType*>* argTypes = new std::vector<ZType*>();
    for (ZArg* arg : *args) {
        argTypes->push_back(arg->getRawType());
        _symTable.addSymbol(arg->getRawType(), arg->getName(), SymbolType::StackVar);
    }

    ZType* funcType = new ZFuncType(retType, *argTypes, typeParams);

    ZBlock* body = nullptr;
    if (!isExtern)
        body = parseBlock();

    SymbolRef* scopeRef = _symTable.makeRef();

    _symTable.exit();

    _symTable.addSymbol(funcType, name, isMethod ? Method : GlobalFunc);

    auto zfunc = new ZFunc(name, retType, *args, *typeParams, body, scopeRef, isExtern);
    zfunc->setType(funcType);

    zfunc->withSourceRange(endRange(sr));

    return zfunc;
}

ZExpr* ZParser::parseLambda() {
    int pos = _lexer.getPos();

    auto sr = beginRange();

    if (!isNext(OPEN_PAREN) && !isNext(IDENT)) {
        _lexer.backtrackTo(pos);
        return parseAssign();
    }

    std::vector<ZArg*>* args = new std::vector<ZArg*>();
    ZType* retType = Unknown;

    if (consume(OPEN_PAREN)) {
        while (!consume(CLOSE_PAREN)) {
            ZArg* arg = parseVagueArg();
            if (!arg) {
                _lexer.backtrackTo(pos);
                return parseAssign();
            }
            args->push_back(arg);
            consume(COMMA);
        }

        std::vector<ZType*>* argTypes = new std::vector<ZType*>();
        for (ZArg* arg : *args)
            argTypes->push_back(arg->getType());

        retType = Unknown;
    }
    else {
        string* id = val(IDENT);
        args->push_back(new ZArg(Unknown, id, _symTable.makeRef()));
    }

    if (!consume(FAT_ARROW)) {
        _lexer.backtrackTo(pos);
        return parseAssign();
    }

    _symTable.enter();

    for (ZArg* arg : *args)
        _symTable.addSymbol(arg->getType(), arg->getName(), SymbolType::StackVar);

    ZAst* body;
    if (isNext(OPEN_BRACE)) {
        body = parseBlock();
        retType = Void;
    }
    else
        body = parseExpr();

    SymbolRef* ref = _symTable.makeRef();

    _symTable.exit();

    auto lambda = new ZLambda(args, retType, body, ref);

    lambda->setRetType(retType);

    lambda->withSourceRange(endRange(sr));
    return lambda;
}

ZArg* ZParser::parseFullArg() {
    std::string* name = val(IDENT);
    if (!name)
        return nullptr;

    if (!consume(COLON))
        return nullptr;

    ZType* type = parseType();

    return new ZArg(type, name, _symTable.makeRef());
}

ZArg* ZParser::parseVagueArg() {
    std::string* name = val(IDENT);
    if (!name)
        return nullptr;

    ZType* type;
    if (consume(COLON))
        type = parseType();
    else
        type = Unknown;

    return new ZArg(type, name, _symTable.makeRef());
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

    auto typeName = *reqVal(IDENT);
    auto type = _symTable.makeRef()->findTypeOrDelayed(typeName);


    auto typeWithGenerics = type->copyStem();
    if (consume(OPEN_BRACKET)) {
        int i = 0;
        while (!consume(CLOSE_BRACKET))
            typeWithGenerics->setTypeParam(i++, parseType());
    }

    if (!consume(FAT_ARROW))
        return typeWithGenerics;

    ZType* retType = parseType();

    return new ZFuncType(retType, std::vector<ZType*>() = {typeWithGenerics});
}

ZBlock* ZParser::parseBlock() {
    auto sr = beginRange();

    std::vector<ZAst*>* stmts = new std::vector<ZAst*>;

    reqConsume(OPEN_BRACE);

    while (!consume(CLOSE_BRACE)) {
        try {
            ZAst* stmt = parseBlockOrStatement();
            stmts->push_back(stmt);
        }
        catch (RecoveryException stmtError) {
            _lexer.recoverToNextStatement();
        }
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

    auto ref = _symTable.addSymbol(type, name, SymbolType::StackVar);

    ZVarDef* zvardef = new ZVarDef(*name, *ref, type, initExpr);

    zvardef->withSourceRange(endRange(sr));

    return zvardef;
}

ZExpr* ZParser::parseAssign() {
    auto sr = beginRange();

    int pos = _lexer.getPos();

    ZExpr* left = parseCall();

    if (left && consume(EQUAL)) {
        auto right = parseExpr();
        auto zassign = new ZAssign(left, right, _symTable.makeRef());
        zassign->withSourceRange(endRange(sr));
        return zassign;
    }

    _lexer.backtrackTo(pos);
    return parseBinOp();
}

ZExpr* ZParser::parseBinOp() {
    int pos = _lexer.getPos();

    auto sr = beginRange();

    std::vector<ZExpr*> operands;
    std::vector<BinOps> operators;

    for (;;) {
        ZExpr* operand = parseAsCast();
        operands.push_back(operand);

        int prePos = _lexer.getPos();
        auto next = _lexer.getNextToken();
        if (_binOps.find(next) == _binOps.end()) {
            _lexer.backtrackTo(prePos);
            break;
        }

        BinOps op = _binOps[next];

        if (operators.size() > 0)
            if (priority(op) < priority(*operators.rbegin())) {
                ZExpr* right = *operands.rbegin();
                operands.pop_back();
                ZExpr* left = *operands.rbegin();
                operands.pop_back();

                BinOps oldOperator = *operators.rbegin();
                operators.pop_back();

                auto newExpr = new ZBinOp(left, right, oldOperator, _symTable.makeRef());
                operands.push_back(newExpr);
            }

        operators.push_back(op);
    }

    while (operands.size() > 1) {
        ZExpr* right = *operands.rbegin();
        operands.pop_back();
        ZExpr* left = *operands.rbegin();
        operands.pop_back();

        BinOps oldOperator = *operators.rbegin();
        operators.pop_back();

        auto newExpr = new ZBinOp(left, right, oldOperator, _symTable.makeRef());
        operands.push_back(newExpr);

    }

    auto result = operands[0];

    result->withSourceRange(endRange(sr));

    return result;
}

ZExpr* ZParser::parseAsCast() {
    auto sr = beginRange();
    ZExpr* expr = parseUnaryOp();

    if (!consume(AS))
        return expr;

    ZType* toType = parseType();

    ZCast* zcast = new ZCast(expr, toType, _symTable.makeRef());
    zcast->withSourceRange(endRange(sr));

    return zcast;
}

ZExpr* ZParser::parseUnaryOp() {
    int pos = _lexer.getPos();
    auto sr = beginRange();

    auto next = _lexer.getNextToken();

    if (_unaryOps.find(next) == _unaryOps.end()) {
        _lexer.backtrackTo(pos);
        ZExpr* target = parseCall();
        int afterTargetPos = _lexer.getPos();
        ZLexeme nextToken = _lexer.getNextToken();
        if (nextToken == DOUBLE_PLUS) {
            ZExpr* unaryOp = new ZUnaryOp(target, PostIncrement);
            unaryOp->withSourceRange(endRange(sr));
            return unaryOp;
        }
        if (nextToken == DOUBLE_MINUS) {
            ZExpr* unaryOp = new ZUnaryOp(target, PostDecrement);
            unaryOp->withSourceRange(endRange(sr));
            return unaryOp;
        }
        _lexer.backtrackTo(afterTargetPos);
        return target;
    }

    auto unaryOp = new ZUnaryOp(parseCall(), _unaryOps[next]);
    unaryOp->withSourceRange(endRange(sr));
    return unaryOp;
}

ZExpr* ZParser::parseCall() {
    int pos = _lexer.getPos();
    // TODO: use expr here

    auto sr = beginRange();

    ZExpr* callee = parseSelector();

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
        return parseSelector();
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

ZExpr* ZParser::parseSelector() {
    auto sr = beginRange();

    auto target = parseId();
    if (!consume(DOT))
        return target;
    auto member = reqVal(IDENT);

    auto selector = new ZSelector(target, member, _symTable.makeRef());
    selector->withSourceRange(endRange(sr));
    return selector;
}

ZExpr* ZParser::parseId() {
    auto sr = beginRange();

    std::string* name = val(IDENT);
    if (!name)
        return parseSizeOf();
    SymbolRef* ref = _symTable.makeRef();

    ZExpr* zid = new ZId(*name, ref);

    zid->withSourceRange(endRange(sr));

    return zid;
}

ZExpr* ZParser::parseSizeOf() {
    auto sr = beginRange();
    if (!consume(SIZEOF))
        return parseString();
    reqConsume(OPEN_BRACKET);

    ZType* type = parseType();

    reqConsume(CLOSE_BRACKET);

    auto zsizeof = new ZSizeOf(type);
    zsizeof->withSourceRange(endRange(sr));

    return zsizeof;
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
        return parseDouble();


    std::string* value = val(CHAR_LIT);
    if (!value)
        return nullptr;

    auto zcharlit = new ZCharLit((*value)[0]);

    zcharlit->withSourceRange(endRange(sr));

    return zcharlit;
}

ZExpr* ZParser::parseDouble() {
    auto sr = beginRange();

    std::string* value = val(DOUBLE_LIT);
    if (!value)
        return parseInt();

    auto zdoublelit = new ZDoubleLit(std::stod((*value).c_str()));

    zdoublelit->withSourceRange(endRange(sr));

    return zdoublelit;
}

ZExpr* ZParser::parseInt() {
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
    if (consume(BOOL_TRUE_LIT))
        return new ZBooleanLit(true);
    else if (consume(BOOL_FALSE_LIT))
        return new ZBooleanLit(false);
    return nullptr;
    //auto pos = endRange(beginRange())->getPosition();
    //throw std::exception(std::string("Unable to recognize an expression starting with " + toString(_lexer.getNextToken())
    //	+ " at " + pos).c_str());
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
        error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()), _lexer.beginRange());
    }
}

std::string* ZParser::reqVal(::ZLexeme lexeme) {
    std::string* value = val(lexeme);
    if (value)
        return value;
    else {
        error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()), _lexer.beginRange());
    }

}

SourceRange* ZParser::beginRange() {
    return _lexer.beginRange();
};

SourceRange* ZParser::endRange(SourceRange* sr) {
    return _lexer.endRange(sr);
};

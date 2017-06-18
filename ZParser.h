﻿#pragma once
#include <string>
#include "ZModule.h"
#include "ZLexer.h"
#include "SymbolTable.h"
//#include "ZArg.h"
#include "ZFunc.h"
#include "ZBlock.h"
#include "ZCall.h"
#include "ZVarDef.h"
#include "ZString.h"

class SymbolTable;
class ZArg;

class ZParser {
public:
    ZParser(ZLexer& lexer, SymbolTable& symTable) 
        : _lexer(lexer), _symTable(symTable) {
        _types["Int"] = Int;
        _types["String"] = String;
        _types["Bool"] = Boolean;
        _types["Double"] = Double;
        _types["None"] = BaseTypes::None;
    }

    ZModule* parseModule() {


        ZModule* module = new ZModule;
        ZFunc* func;
        while (func = parseFunc())
            module->addFunction(func);

        return module;
    }

    ZFunc* parseFunc() {
        reqConsume(DEF);
        std::string* name = reqVal(IDENT);
        reqConsume(OPEN_PAREN);

        std::vector<ZArg*>* args = new std::vector<ZArg*>();
        while (!consume(CLOSE_PAREN)) {
            args->push_back(parseArg());
            consume(COMMA);
        }

        reqConsume(COLON);

        BaseTypes retType = parseType();

        reqConsume(EQUAL);

        ZBlock* body = parseBlock();

        return new ZFunc(name, retType, *args, body);
    }

    ZArg* parseArg() {
        std::string* name = reqVal(IDENT);
        reqConsume(COLON);
        BaseTypes type = parseType();

        return new ZArg(type, name);
    }

    BaseTypes parseType() {
        return _types[*reqVal(IDENT)];
    }

    ZBlock* parseBlock() {
        std::vector<ZAst*>* stmts = new std::vector<ZAst*>;

        while (!consume(CLOSE_BRACE)) {
            ZAst* stmt = parseStatement();
            reqConsume(SEMICOLON);
            stmts->push_back(stmt);
        }

        return new ZBlock(stmts);
    }

    ZAst* parseStatement() {
        int pos = _lexer.getPos();
        if (consume(VAR)) {
            _lexer.backtrackTo(pos);
            return parseVarDef();
        }

        return parseExpr();
    }

    ZVarDef* parseVarDef() {
        reqConsume(VAR);
        std::string* name = reqVal(IDENT);
        reqConsume(COLON);
        BaseTypes type = parseType();
        // TODO: add optional init expr
        // TODO:: add symbol table stuff
        return new ZVarDef(*name, type);
    }

    ZExpr* parseExpr() {
        return parseString();
    }

    ZCall* parseCall() {
        // TODO: use expr here
        ZExpr* callee = parseId();
        reqConsume(OPEN_PAREN);

        std::vector<ZExpr*>* args = new std::vector<ZExpr*>();
        while (!consume(CLOSE_PAREN)) {
            ZExpr* arg = parseExpr();
            consume(COMMA);
            args->push_back(arg);
        }

        return new ZCall(callee, *args);
    }

    ZId* parseId() {
        std::string* name = val(IDENT);
        if (!name)
            return nullptr;

        return new ZId(*name);
    }

    ZString* parseString() {
        std::string* value = val(STRING_LIT);
        if (!value)
            return nullptr;

        return new ZString(*value);
    }

    void reqConsume(ZLexeme lexeme) {
        if (!consume(lexeme))
            error("Expected: " + _lexer.toString(lexeme) + ", but found: " + _lexer.toString(_lexer.getNextToken()));
    }

    bool consume(ZLexeme lexeme) {
        int pos = _lexer.getPos();
        
        if (_lexer.getNextToken() == lexeme)
            return true;
        else {
            _lexer.backtrackTo(pos);
            return false;
        }
    }

    std::string* val(ZLexeme lexeme) {
        if (consume(lexeme))
            return _lexer.getValue();
        else
            return nullptr;
    }

    std::string* reqVal(ZLexeme lexeme) {
        std::string* value = val(lexeme);
        if (value)
            return value;
        else
            error("Expected: " + _lexer.toString(lexeme) + ", but found: " + _lexer.toString(_lexer.getNextToken()));

    }

    void error(std::string& errorText) {
        throw new std::exception(errorText.c_str());
    }

private:
    ZLexer _lexer;
    SymbolTable _symTable;

    std::map<std::string, BaseTypes> _types;
};

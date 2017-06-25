#pragma once
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
#include "ZAssign.h"
#include "ZBinOp.h"

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
		auto modName = new std::string("test"); // TODO: user real mod name
        ZModule* module = new ZModule(*modName);
        ZFunc* func;
        while (func = parseFunc())
            module->addFunction(func);

        return module;
    }

    ZFunc* parseFunc() {
		if (!consume(DEF))
			return nullptr;
	    
	    (DEF);
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

		reqConsume(OPEN_BRACE);

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
        // TODO: add symbol table stuff
        return new ZVarDef(*name, type);
    }

    ZExpr* parseExpr() {
        return parseAssign();           
        
    }

    ZExpr* parseAssign() {
        int pos = _lexer.getPos();

        ZExpr* left = parseId();

        if (left && consume(EQUAL))
            return new ZAssign(left, parseAssign());
        
        _lexer.backtrackTo(pos);
        return parseBinOp();
    }

    ZExpr* parseBinOp() {
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

    ZExpr* parseCall() {
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

    ZExpr* parseId() {
        std::string* name = val(IDENT);
        if (!name)
            return parseString();

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
            error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()));
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
            error("Expected: " + toString(lexeme) + ", but found: " + toString(_lexer.getNextToken()));

    }

    void error(std::string& errorText) {
        throw new std::exception(errorText.c_str());
    }

private:
    ZLexer _lexer;
    SymbolTable _symTable;

    std::map<std::string, BaseTypes> _types;
};

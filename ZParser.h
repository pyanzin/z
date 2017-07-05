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
#include "ZAssign.h"
#include "ZBinOp.h"
#include "ZIntLit.h"
#include "ZId.h"

class SymbolTable;
class ZArg;

class ZParser {
public:
    ZParser(ZLexer& lexer, SymbolTable& symTable) 
        : _lexer(lexer), _symTable(symTable) {
        _types["Int"] = Int;
        _types["String"] = String;
        _types["Boolean"] = Boolean;
        _types["Double"] = Double;
		_types["None"] = None;
    }

    ZModule* parseModule() {
		auto modName = new std::string("test"); // TODO: user real mod name
        ZModule* module = new ZModule(*modName);
        ZFunc* func;
        while (func = parseFunc())
            module->addFunction(func);

        return module;
    }

	ZFunc* parseFunc();

	ZArg* parseArg();

	ZType* parseType() {
        return _types[*reqVal(IDENT)];
    }

	ZBlock* parseBlock();

	ZAst* parseReturn();

	ZAst* parseStatement();

	ZVarDef* parseVarDef();

    ZExpr* parseExpr() {
        return parseAssign();           
        
    }

	ZExpr* parseAssign();

	ZExpr* parseBinOp();

	ZExpr* parseCall();

	ZExpr* parseId();

	ZExpr* parseString();

	ZExpr* parseNumber();

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

	std::map<std::string, ZType*> _types;
};

#pragma once
#include <string>
#include "ZModule.h"
#include "SymbolTable.h"
#include "ZFunc.h"
#include "ZBlock.h"
#include "ZVarDef.h"
#include "ZLexer.h"

class SymbolTable;
class ZArg;
class SourceRange;
class ZLexer;
enum ZLexeme;
enum BinOps;

class ZParser {
public:
    ZParser(ZLexer& lexer, SymbolTable& symTable);

    ZModule* parseModule();

    ZFunc* parseFunc();
	ZExpr* parseLambda();
	ZArg* parseArg();

	ZType* parseType();

	ZBlock* parseBlock();

	ZAst* parseReturn();

	ZAst* parseIf();

	ZAst* parseWhile();

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
    ZExpr* parseBoolean();
    ZExpr* parseChar();
	void reqConsume(ZLexeme lexeme);

    bool consume(ZLexeme lexeme);
	bool isNext(::ZLexeme lexeme);
	std::string* val(ZLexeme lexeme);

    std::string* reqVal(ZLexeme lexeme);

    void error(std::string errorText) {
        throw new std::exception(errorText.c_str());
    }

    SourceRange* beginRange();

    SourceRange* endRange(SourceRange* sr);

private:

    ZLexer _lexer;
    SymbolTable _symTable;

	std::map<std::string, ZType*> _types;

	std::map<ZLexeme, BinOps> _ops;
};

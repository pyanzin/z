#pragma once
#include <string>
#include "ZModule.h"
#include "SymbolTable.h"
#include "ZFunc.h"
#include "ZBlock.h"
#include "ZVarDef.h"
#include "ZLexer.h"
#include "ZUnaryOp.h"
#include "ParserError.h"
#include "RecoveryException.h"

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

    void parseStruct();

    void parseClass();

    ZArg* parseField();

    ZFunc* parseFunc(bool isMethod = false);
    ZExpr* parseLambda();
    ZArg* parseFullArg();
    ZArg* parseVagueArg();
    ZType* parseType();

    ZBlock* parseBlock();
    ZAst* parseBlockOrStatement();
    ZAst* parseReturn();

    ZAst* parseIf();

    ZAst* parseWhile();

    ZAst* parseFor();

    ZAst* parseStatement();

    ZVarDef* parseVarDef();

    ZExpr* parseExpr() {
        return parseLambda();
    }

    ZExpr* parseAssign();

    ZExpr* parseBinOp();

    ZExpr* parseCall();

    ZExpr* parseUnaryOp();

    ZExpr* parseSelector();
    ZExpr* parseId();

    ZExpr* parseSizeOf();
    ZExpr* parseAsCast();

    ZExpr* parseString();

    ZExpr* parseInt();
    ZExpr* parseBoolean();
    ZExpr* parseChar();
    ZExpr* parseDouble();
    void reqConsume(ZLexeme lexeme);

    bool consume(ZLexeme lexeme);
    bool isNext(::ZLexeme lexeme);
    std::string* val(ZLexeme lexeme);

    std::string* reqVal(ZLexeme lexeme);

    int priority(BinOps op) {
        return _binOpPriority[op];
    }

    void error(std::string errorText, SourceRange* sr) {
        addError(errorText, sr);
        throw RecoveryException();
    }

    SourceRange* beginRange();

    SourceRange* endRange(SourceRange* sr);

    void addError(std::string error, SourceRange* sr) {
        errors.push_back(new ParserError(error, sr));
    }

    std::vector<ParserError*> getErrors() {
        return errors;
    }

private:
    ZLexer _lexer;
    SymbolTable _symTable;

    ZModule* _module;

    std::map<std::string, ZType*> _types;

    std::map<ZLexeme, BinOps> _binOps;
    std::map<BinOps, int> _binOpPriority;
    std::map<ZLexeme, UnaryOps> _unaryOps;

    std::vector<ParserError*> errors;
};

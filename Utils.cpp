#include "ZBasicType.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include "ZLexeme.h"
#include "ZBinOp.h"

std::string toString(ZLexeme lexeme) {
	switch (lexeme) {
	case IDENT:         return "IDENT";
	case DEF:           return "DEF";
	case EXTERN:        return "EXTERN";
	case VAR:           return "VAR";
	case RETURN:        return "RETURN";
	case IF:            return "IF";
	case ELSE:          return "ELSE";
	case WHILE:         return "WHILE";
	case STRING_LIT:    return "STRING_LIT";
	case INT_LIT:       return "INT_LIT";
	case CHAR_LIT:      return "CHAR_LIT";
	case OPEN_PAREN:    return "OPEN_PAREN";
	case CLOSE_PAREN:   return "CLOSE_PAREN";
	case COLON:         return "COLON";
	case SEMICOLON:     return "SEMICOLON";
	case EQUAL:         return "EQUAL";
	case DOUBLE_EQUAL:  return "DOUBLE_EQUAL";
	case NOT_EQUAL:     return "NOT_EQUAL";
	case LESS:          return "LESS";
	case LESS_OR_EQUAL: return "LESS_OR_EQUAL";
	case MORE:          return "MORE";
	case MORE_OR_EQUAL: return "MORE_OR_EQUAL";
	case OPEN_BRACE:    return "OPEN_BRACE";
	case CLOSE_BRACE:   return "CLOSE_BRACE";
	case FAT_ARROW:     return "FAT_ARROW";
	case COMMA:         return "COMMA";
	case PLUS:          return "PLUS";
	case MINUS:         return "MINUS";
	case ASTERISK:      return "ASTERISK";
	case EXCLAM:        return "EXCLAM";
	case SLASH:         return "SLASH";
	case INPUT_END:     return "INPUT_END";

	}

}

std::string toString(BinOps op) {
	switch (op) {
	case BinOps::Sum:
		return "+";
	case BinOps::Sub:
		return "-";
	case BinOps::Mul:
		return "*";
	case BinOps::Div:
		return "/";
	case BinOps::Equal:
		return "==";
	case BinOps::NotEqual:
		return "!=";
	case BinOps::Less:
		return "<";
	case BinOps::LessOrEqual:
		return "<=";
	case BinOps::More:
		return ">";
	case BinOps::MoreOrEqual:
		return ">=";
	}
}
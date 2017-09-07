#include "ZBasicTypes.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include "ZLexeme.h"
#include "ZBinOp.h"

std::string toString(ZLexeme lexeme) {
	switch (lexeme) {
	case IDENT:
		return "IDENT";
	case DEF:
		return "DEF";
	case VAR:
		return "VAR";
	case STRING_LIT:
		return "STRING_LIT";
	case OPEN_PAREN:
		return "OPEN_PAREN";
	case CLOSE_PAREN:
		return "CLOSE_PAREN";
	case COLON:
		return "COLON";
	case SEMICOLON:
		return "SEMICOLON";
	case EQUAL:
		return "EQUAL";
	case DOUBLE_EQUAL:
		return "DOUBLE_EQUAL";
	case OPEN_BRACE:
		return "OPEN_BRACE";
	case CLOSE_BRACE:
		return "CLOSE_BRACE";
	case FAT_ARROW:
		return "FAT_ARROW";
	case COMMA:
		return "COMMA";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case ASTERISK:
        return "ASTERISK";
    case SLASH:
        return "SLASH";
	case INPUT_END:
		return "INPUT_END";

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
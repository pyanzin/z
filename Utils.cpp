#include "ZBasicTypes.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include "ZLexeme.h"
#include "ZBinOp.h"

llvm::Type* toLlvmType(BaseTypes ztype) {
	switch (ztype)
	{
	case Int:
		return llvm::IntegerType::get(llvm::getGlobalContext(), 32);
	case Double:
		break;
	case Boolean:
		return llvm::IntegerType::get(llvm::getGlobalContext(), 1);
	case String:
	default:
		return nullptr;
	}
}

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
	case INPUT_END:
		return "INPUT_END";

	}

}


std::string toString(BaseTypes type) {
	switch (type) {
	case Boolean:
		return "Boolean";
	case Int:
		return "Int";
	case Double:
		return "Double";
	case String:
		return "String";
	case BaseTypes::None:
		return "None";
	case Unknown:
		return "Unknown";
	}
}

std::string toString(BinOps op) {
	switch (op) {
	case BinOps::Sum:
		return "Sum";
	case BinOps::Sub:
		return "Sub";
	case BinOps::Mul:
		return "Mul";
	case BinOps::Div:
		return "Div";
	}
}
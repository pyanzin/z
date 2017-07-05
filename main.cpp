#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <cstdio>
#include <iostream>
#include "ZLexer.h"
#include "ZParser.h"
#include "TypingPass.h"
#include "LlvmPass.h"


//using namespace llvm;

llvm::Module* makeLLVMModule();

int main() {
	//auto args = std::vector<ZArg*>() = { new ZArg(Int, "x"), new ZArg(Int, "y") };
	//auto myFunc = new ZFunc(new std::string("MyFunc"), Int, args, new ZBinOp(new ZId("x"), new ZId("y"), Sum));

	//Module* mod = new Module("test", context);

	//myFunc->generateDef(mod);

	/*Function* sum = cast<Function>(c);
	sum->setCallingConv(CallingConv::C);

	Function::arg_iterator args = sum->arg_begin();
	Argument* x = (args++).getNodePtrUnchecked();
	x->setName("x");
	auto y = args.getNodePtrUnchecked();
	y->setName("y");

	auto bb = BasicBlock::Create(getGlobalContext(), "entry", sum);

	builder.SetInsertPoint(bb);

	auto ast = new ZBinOp(new ZIntLit(23), new ZIntLit(19), Sum);

	auto result = ast->codegen();

	builder.CreateRet(result);
	*/

	//auto mod = makeLLVMModule();
	//verifyModule(*mod);

	//mod->dump();

	//std::string src = "def main(params: String): None = {"
	//	"var name: String = readLine();"
	//	"var age: Int = toInt(readLine());"
	//	"print(\"hi \" + name);"
	//	"print(\"you will die in approx. \" + (75 - age));}";

	std::string src = "def sum(a: Int, b: Int): Boolean = {"
		"var x: Int = sum(a-1, b);"
		"return x == (a - b);"
		"} ";

    ZLexer lexer(src);

    //ZLexeme lexeme;
    //while ((lexeme = lexer.getNextToken()) != INPUT_END)
    //    printf("%d %s\n", (int)lexeme, lexer.getValue()->c_str());

	ZType* readLineType = new ZFuncType(String, std::vector<ZType*>());
	ZType* toIntType = new ZFuncType(Int, std::vector<ZType*>() = { String });
	ZType* printType = new ZFuncType(None, std::vector<ZType*>() = { String });

    SymbolTable table;

	table.add(readLineType, new std::string("readLine"));
	table.add(toIntType, new std::string("toInt"));
	table.add(printType, new std::string("print"));

    ZParser parser(lexer, table);

    auto mod = parser.parseModule();
	mod->dump(std::cout, 0);

	getchar();

	TypingPass typingPass;
	typingPass.visit(mod);
	mod->dump(std::cout, 0);

    getchar();

	LlvmPass llvmPass;

	llvmPass.visit(mod);

	llvmPass.getModule()->dump();

	getchar();

    return 0;
}

//int allocate(ZType* type) {	auto size = type->size();
//	if (lastAllocPtr + size > memEnd)	
//		gc();	
//
//	auto result = lastAllocPtr;
//	lastAllocPtr += size;
//
//	return result;
//}



//class ZType
//{
//public:
//	ZField* fields;
//	char size;
//
//	ZType(ZField* flds)
//	{
//		char s = 0;
//
//	}
//};
//
//struct ZField
//{
//	char size;
//};
//
//Module* makeLLVMModule() {
//	Module* mod = new Module("test", getGlobalContext());
//
//	Constant* c = mod->getOrInsertFunction("gcd",
//		IntegerType::get(getGlobalContext(), 32),
//		IntegerType::get(getGlobalContext(), 32),
//		IntegerType::get(getGlobalContext(), 32),
//		NULL);
//	Function* gcd = cast<Function>(c);
//
//	Function::arg_iterator args = gcd->arg_begin();
//	Value* x = args++;
//	x->setName("x");
//	Value* y = args++;
//	y->setName("y");
//
//	BasicBlock* entry = BasicBlock::Create(getGlobalContext(), "entry", gcd);
//	BasicBlock* ret = BasicBlock::Create(getGlobalContext(), "return", gcd);
//	BasicBlock* cond_false = BasicBlock::Create(getGlobalContext(), "cond_false", gcd);
//	BasicBlock* cond_true = BasicBlock::Create(getGlobalContext(), "cond_true", gcd);
//	BasicBlock* cond_false_2 = BasicBlock::Create(getGlobalContext(), "cond_false", gcd);
//
//	IRBuilder<> builder(entry);
//	Value* xEqualsY = builder.CreateICmpEQ(x, y, "tmp");
//	builder.CreateCondBr(xEqualsY, ret, cond_false);
//
//	builder.SetInsertPoint(ret);
//	builder.CreateRet(x);
//
//	builder.SetInsertPoint(cond_false);
//	Value* xLessThanY = builder.CreateICmpULT(x, y, "tmp");
//	builder.CreateCondBr(xLessThanY, cond_true, cond_false_2);
//
//	builder.SetInsertPoint(cond_true);
//	Value* yMinusX = builder.CreateSub(y, x, "tmp");
//	std::vector<Value*> args1;
//	args1.push_back(x);
//	args1.push_back(yMinusX);
//	Value* recur_1 = builder.CreateCall(gcd, args1, "tmp");
//	builder.CreateRet(recur_1);
//
//	builder.SetInsertPoint(cond_false_2);
//	Value* xMinusY = builder.CreateSub(x, y, "tmp");
//	std::vector<Value*> args2;
//	args2.push_back(xMinusY);
//	args2.push_back(y);
//	Value* recur_2 = builder.CreateCall(gcd, args2, "tmp");
//	builder.CreateRet(recur_2);
//
//	return mod;
//}
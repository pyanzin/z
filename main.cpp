#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include <cstdio>
#include <vector>
#include <iostream>
#include "ZLexer.h"
#include "ZBasicTypes.h"
#include "ZFunc.h"
#include "ZExpr.h"
#include "Utils.h"
#include "ZArg.h"
#include "ZBinOp.h"
#include "ZIntConst.h"
#include "ZVar.h"
#include "ZExpr.h"

using namespace llvm;

int main()
{
    LLVMContext& context = getGlobalContext();

    auto args = std::vector<ZArg*>() = { new ZArg(Int, "x"), new ZArg(Int, "y") };
    auto myFunc = new ZFunc(new std::string("MyFunc"), Int, args, new ZBinOp(new ZVar("x"), new ZVar("y"), Sum));

    Module* mod = new Module("test", context);

    myFunc->generateDef(mod);

    /*Function* sum = cast<Function>(c);
    sum->setCallingConv(CallingConv::C);

    Function::arg_iterator args = sum->arg_begin();
    Argument* x = (args++).getNodePtrUnchecked();
    x->setName("x");
    auto y = args.getNodePtrUnchecked();
    y->setName("y");

    auto bb = BasicBlock::Create(getGlobalContext(), "entry", sum);

    builder.SetInsertPoint(bb);

    auto ast = new ZBinOp(new ZIntConst(23), new ZIntConst(19), Sum);

    auto result = ast->codegen();

    builder.CreateRet(result);
  */
    verifyModule(*mod);

    mod->dump();

    std::string src = "def main(): None = {print(\"hallo\");}";

    ZLexer lexer(src);

    ZLexeme lexeme;
    while ((lexeme = lexer.getNextToken()) != INPUT_END)
        printf("%d %s\n", (int)lexeme, lexer.getValue().c_str());

    auto visitor = new ZVisitor();
    ZArg* arg = new ZArg(Int, "xxx");
    auto i = new ZIntConst(200);

    arg->accept(visitor);
    i->accept(visitor);

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

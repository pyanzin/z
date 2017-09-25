#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <cstdio>
#include <iostream>
#include "ZLexer.h"
#include "ZParser.h"
#include "TypingPass.h"
#include "LlvmPass.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <fstream>
#include <sstream>

//using namespace llvm;

llvm::Module* makeLLVMModule();

int main(int argc, char* args[]) {
    try {

        std::ifstream srcFile;
        std::stringstream stream;
        srcFile.open(args[1]);
        stream << srcFile.rdbuf();
        std::string src = stream.str();

        ZLexer lexer(&src);

        SymbolTable table;

        ZParser parser(lexer, table);

        auto mod = parser.parseModule();
        mod->dump(std::cout, 0);

        //getchar();

        TypingPass typingPass;
        typingPass.visit(mod);
        mod->dump(std::cout, 0);

        //getchar();

        LlvmPass llvmPass;

        llvmPass.visit(mod);

        auto module = llvmPass.getModule();
        module->dump();

        verifyModule(*module);

        //getchar();

        auto fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(module);
        // Promote allocas to registers.
        fpm->add(llvm::createPromoteMemoryToRegisterPass());
        //// Do simple "peephole" optimizations and bit-twiddling optzns.
        fpm->add(llvm::createInstructionCombiningPass());
        // Reassociate expressions.
        fpm->add(llvm::createReassociatePass());
        //// Eliminate Common SubExpressions.
        fpm->add(llvm::createGVNPass());
        //// Simplify the control flow graph (deleting unreachable blocks, etc).
        fpm->add(llvm::createCFGSimplificationPass());

        fpm->doInitialization();

        fpm->run(*module->getFunction("main"));

        module->dump();

        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        auto targetTriple = sys::getDefaultTargetTriple();
        std::string error;
        auto target = TargetRegistry::lookupTarget(targetTriple, error);

        if (!target) {
            errs() << Module::Error;
            return 1;
        }

        auto CPU = "generic";
        auto Features = "";

        TargetOptions opt;
        auto RM = Reloc::Model();
        auto TargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);

        module->setDataLayout(TargetMachine->createDataLayout());
        module->setTargetTriple(targetTriple);

        auto Filename = "output.o";
        std::error_code EC;
        raw_fd_ostream dest(Filename, EC, sys::fs::F_None);

        if (EC) {
            errs() << "Could not open file: " << EC.message();
            return 1;
        }

        legacy::PassManager pass;
        auto FileType = TargetMachine::CGFT_ObjectFile;

        if (TargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
            errs() << "TargetMachine can't emit a file of this type";
            return 1;
        }

        pass.run(*module);
        dest.flush();

        //getchar();

    }
    catch (std::exception ex) {
        cout << "Error: " + std::string(ex.what()) << '\n';
        //getchar();
    }
    return 0;
}

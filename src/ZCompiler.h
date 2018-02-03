#pragma once
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <fstream>
#include <iostream>
#include "ZLexer.h"
#include "ZParser.h"
#include "TypingPass.h"
#include "LlvmPass.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include <fstream>
#include <sstream>
#include "ZCompilerSettings.h"

using namespace llvm;

class ZCompiler {
public:
	ZCompiler(ZCompilerSettings* settings) {
		_settings = settings;
	}

	void run() {
		try {
			std::ifstream srcFile;
			std::stringstream stream;
			srcFile.open(_settings->sources[0]);
			stream << srcFile.rdbuf();
			std::string src = stream.str();

			ZLexer lexer(&src);

			SymbolTable table;

			ZParser parser(lexer, table);

			auto mod = parser.parseModule();

			if (_settings->dumpParserTree)
				mod->dump(std::cout, 0);

			TypingPass typingPass;
			typingPass.visit(mod);
            
			if (_settings->dumpTypingTree)
				mod->dump(std::cout, 0);

            int errorsCount = parser.getErrors().size() + typingPass.getErrors().size();

            if (errorsCount > 0) {
                cout << "Compilation failed, " << errorsCount << " error(s) found:\n";
                for (auto error : parser.getErrors())
                    cout << error->text << " at position " << error->sourceRange->getPosition() << "\n";
                for (auto error : typingPass.getErrors())
                    cout << error->text << " at position " << error->sourceRange->getPosition() << "\n";
                return;
            }

			LlvmPass llvmPass;
			llvmPass.visit(mod);
			auto module = llvmPass.getModule();

			if (_settings->dumpLlvmBeforeOpt)
				module->dump();

			if (_settings->optLevel >= 1) {
				auto fpm = llvm::make_unique<llvm::legacy::FunctionPassManager>(module);
				fpm->add(llvm::createPromoteMemoryToRegisterPass());
				fpm->add(llvm::createInstructionCombiningPass());
				fpm->add(llvm::createReassociatePass());
				fpm->add(llvm::createGVNPass());
				fpm->add(llvm::createCFGSimplificationPass());

				fpm->doInitialization();

				fpm->run(*module->getFunction("main"));
			}

			if (_settings->dumpLlvmAfterOpt)
				module->dump();

			InitializeAllTargetInfos();
			InitializeAllTargets();
			InitializeAllTargetMCs();
			InitializeAllAsmParsers();
			InitializeAllAsmPrinters();

			auto targetTriple = sys::getDefaultTargetTriple();
			std::string error;
			auto target = TargetRegistry::lookupTarget(targetTriple, error);

			auto CPU = "generic";
			auto Features = "";

			TargetOptions opt;
			auto RM = Reloc::Model();
			auto TargetMachine = target->createTargetMachine(targetTriple, CPU, Features, opt, RM);

			module->setDataLayout(TargetMachine->createDataLayout());
			module->setTargetTriple(targetTriple);

			auto Filename = "output.obj";
			std::error_code EC;
            raw_fd_ostream dest(Filename, EC, sys::fs::OpenFlags());

			if (EC) {
				errs() << "Could not open file: " << EC.message();
				return;
			}

			legacy::PassManager pass;
			auto FileType = TargetMachine::CGFT_ObjectFile;

			if (TargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
				errs() << "TargetMachine can't emit a file of this type";
				return;
			}

			pass.run(*module);
			dest.flush();

		}
		catch (std::exception ex) {
			cout << "Error: " + std::string(ex.what()) << '\n';
		}
	}
private:
	ZCompilerSettings* _settings;
};

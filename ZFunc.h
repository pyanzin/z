#ifndef ZFUNC
#define ZFUNC

#include <vector>
#include <llvm/IR/Module.h>
#include "ZExpr.h"
#include "Utils.h"
#include "ZArg.h"

class ZFunc {
public:
    ZFunc(std::string* name, BaseTypes returnType, std::vector<ZArg*>& args, ZExpr* body) : _args(args) {
        _name = name;
        _returnType = returnType;
        _body = body;
    }

    void generateDef(Module* module) {
        auto args = std::vector<Type*>();
        for (auto arg : _args)
            args.push_back(toLlvmType(arg->getType()));

        auto funcType = FunctionType::get(toLlvmType(_returnType), args, false);
        auto func = Function::Create(funcType, Function::ExternalLinkage, _name->c_str(), module);

        unsigned i = 0;
        for (auto& arg : func->args())
            arg.setName(_args[i++]->getName());

        BasicBlock* block = BasicBlock::Create(getGlobalContext(), "entry", func);
        builder.SetInsertPoint(block);

        auto ret = _body->codegen(func);
        builder.CreateRet(ret);
    }

private:
    std::string* _name;
    BaseTypes _returnType;
    std::vector<ZArg*> _args;
    ZExpr* _body;

};

#endif
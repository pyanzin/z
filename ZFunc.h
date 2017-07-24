#pragma once

#include <vector>
#include "ZExpr.h"
#include "ZBlock.h"

class ZArg;

class ZFunc : public ZAst {
public:
	ZFunc(std::string* name, ZType* returnType, std::vector<ZArg*>& args, ZBlock* body) : _args(args) {
        _name = name;
        _returnType = returnType;
        _body = body;
    }

	void accept(ZVisitor* visitor);

    //void generateDef(Module* module) {
    //    auto args = std::vector<Type*>();
    //    for (auto arg : _args)
    //        args.push_back(toLlvmType(arg->getType()));

    //    auto funcType = FunctionType::get(toLlvmType(_returnType), args, false);
    //    auto func = Function::Create(funcType, Function::ExternalLinkage, _name->c_str(), module);

    //    unsigned i = 0;
    //    for (auto& arg : func->args())
    //        arg.setName(_args[i++]->getName());

    //    BasicBlock* block = BasicBlock::Create(getGlobalContext(), "entry", func);
    //    builder.SetInsertPoint(block);

    //    auto ret = _body->codegen(func);
    //    builder.CreateRet(ret);
    //}

	void dump(std::ostream& stream, unsigned depth) override;

//private:
    std::string* _name;
	ZType* _returnType;
    std::vector<ZArg*> _args;
    ZBlock* _body;

};

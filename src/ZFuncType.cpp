#include <llvm/IR/DerivedTypes.h>
#include "ZFuncType.h"
#include "LlvmStorage.h"

llvm::Type* ZFuncType::toLlvmType() {
    std::vector<llvm::Type*>* argTypes = new std::vector<llvm::Type*>();
    for (auto argType : getParamTypes())
        argTypes->push_back(argType->toLlvmType());

    auto funcType = llvm::FunctionType::get(getRetType()->toLlvmType(), *argTypes, false);
    return llvm::PointerType::get(funcType, 0);
}

ZType* ZFuncType::copyStem() {
    auto argTypes = new std::vector<ZType*>();
    for (auto argType : getParamTypes())
        argTypes->push_back(Unknown);
    return new ZFuncType(Unknown, *argTypes);
}

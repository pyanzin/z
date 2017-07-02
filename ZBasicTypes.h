#pragma once
#include <llvm/IR/Type.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>

class ZType {
public:
    virtual llvm::Type* toLlvmType() = 0;

    virtual std::string& toString() = 0;
};

class ZBasicType : public ZType {
public:
    ZBasicType(llvm::Type* type, std::string name) : _name(name) {
        _type = type;
    }

    llvm::Type* toLlvmType() override;

    std::string& toString() override;

private:
    llvm::Type* _type;
    std::string _name;
};

extern ZBasicType* Unknown;
extern ZBasicType* Int;
extern ZBasicType* Double;
extern ZBasicType* Boolean;
extern ZBasicType* String;
extern ZBasicType* None;

class ZFuncType : public ZType {
public:
    ZFuncType(ZType* retType, std::vector<ZType*>& argTypes) {
        _retType = retType;
        _argTypes = argTypes;
    }

    virtual llvm::Type* toLlvmType() {
        return nullptr;
    };

    virtual std::string& toString() {
        return std::string();
    };

private:
    ZType* _retType;
    std::vector<ZType*> _argTypes;
};


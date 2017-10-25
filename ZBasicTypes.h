#pragma once
#include <llvm/IR/Type.h>
#include "ZType.h"

class ZArrayType;

class ZBasicType : public ZType {
public:
    ZBasicType(llvm::Type* type, std::string name) : _name(name) {
        _type = type;
    }

	std::string& getName() override {
		return _name;
	}

    llvm::Type* toLlvmType() override;

    std::string& toString() override;

    bool isEqual(ZType& other) override {
        return this == &other;
    };

private:
    llvm::Type* _type;
    std::string _name;
};

extern ZBasicType* Unknown;
extern ZBasicType* Int;
extern ZBasicType* Double;
extern ZBasicType* Char;
extern ZBasicType* Boolean;
extern ZBasicType* String;
extern ZBasicType* Void;

class ZFuncType : public ZType {
public:
    ZFuncType(ZType* retType, std::vector<ZType*>& argTypes) {
        _typeParams->push_back(retType);
        for (ZType* argType : argTypes)
            _typeParams->push_back(argType);
    }

    bool isEqual(ZType& other) override {
        ZFuncType* otherType = dynamic_cast<ZFuncType*>(&other);
        if (!otherType)
            return false;
        if (this->getParamTypes().size() != otherType->getParamTypes().size())
            return false;

        auto thisArgs = this->getParamTypes();
        auto otherArgs = otherType->getParamTypes();

        for (int i = 0; i != thisArgs.size(); ++i) {
            if (!thisArgs[i]->isEqual(*otherArgs[i]))
                return false;
        }

        return this->getRetType() == otherType->getRetType();
    }

	std::string& getName() override {
		return toString();
	}

	llvm::Type* toLlvmType();

    std::string& toString() override {
		std::string* res = new std::string("(");
		bool isLast = false;
		for (auto i = _typeParams->begin() + 1; i != _typeParams->end(); ++i) {
			isLast = i == _typeParams->end() - 1;
			*res += (*i)->toString();
			if (!isLast)
				*res += ", ";
		}

		*res += ") => " + getRetType()->toString();
		return *res;
    };

	std::vector<ZType*>& getParamTypes() {
		return (*new std::vector<ZType*>(_typeParams->begin() + 1, _typeParams->end()));
	}

	ZType* getRetType() {
		return (*_typeParams)[0];
	}

private:
};

class ZGenericParam : public ZType {
public:
	ZGenericParam(std::string& name) : _name(name) {
		
	}

	llvm::Type* toLlvmType() override {
		return nullptr;
	}

	std::string& getName() override {
		return _name;
	}

	std::string& toString() override {
		return _name;
	}

	bool isEqual(ZType& other) override {
		return &other == this;
	}

private:
	std::string& _name;
};
#pragma once
#include <llvm/IR/Type.h>

class ZType {
public:
    virtual llvm::Type* toLlvmType() = 0;

    virtual std::string& toString() = 0;

    virtual bool isEqual(ZType& other) = 0;
};

class ZBasicType : public ZType {
public:
    ZBasicType(llvm::Type* type, std::string name) : _name(name) {
        _type = type;
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
        _retType = retType;
        _argTypes = argTypes;
    }

    bool isEqual(ZType& other) override {
        ZFuncType* otherType = dynamic_cast<ZFuncType*>(&other);
        if (!otherType)
            return false;
        if (this->getArgTypes().size() != otherType->getArgTypes().size())
            return false;

        auto thisArgs = this->getArgTypes();
        auto otherArgs = otherType->getArgTypes();

        for (int i = 0; i != thisArgs.size(); ++i) {
            if (thisArgs[i] != otherArgs[i])
                return false;
        }

        return this->getRetType() == otherType->getRetType();
    }

	llvm::Type* toLlvmType();

    std::string& toString() override {
		std::string* res = new std::string("(");
		bool isLast = false;
		for (auto i = _argTypes.begin(); i != _argTypes.end(); ++i) {
			isLast = i == _argTypes.end() - 1;
			*res += (*i)->toString();
			if (!isLast)
				*res += ", ";
		}

		*res += ") => " + _retType->toString();
		return *res;
    };

	std::vector<ZType*>& getArgTypes() {
		return _argTypes;
	}

	ZType* getRetType() {
		return _retType;
	}

private:
    ZType* _retType;
    std::vector<ZType*> _argTypes;
};


#pragma once
#include "ZType.h"
#include <vector>
#include "ZGenericParam.h"


class ZFuncType : public ZType {
public:
	ZFuncType(ZType* retType, std::vector<ZType*>& argTypes, std::vector<ZGenericParam*>* genericDefs = nullptr) {
		_typeParams->push_back(retType);
		for (ZType* argType : argTypes)
			_typeParams->push_back(argType);
        _genericDefs = genericDefs ? genericDefs : new std::vector<ZGenericParam*>;
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

    void addGenericDef(ZGenericParam* param) {
        _genericDefs->push_back(param);
	}

    std::vector<ZGenericParam*>* getGenericDefs() {
        return _genericDefs;
	}

    ZType* copyStem() override;

    bool hasGenericDefs() {
        return _genericDefs->size() > 0;
	}

    bool canBeCastedTo(ZType* other) override {
        ZFuncType* otherFunc = dynamic_cast<ZFuncType*>(other);
        if (!otherFunc)
            return false;

        if (!otherFunc->getRetType()->canBeCastedTo(this->getRetType()))
            return false;

        if (otherFunc->getParamTypes().size() != this->getParamTypes().size())
            return false;

        for (int i = 0; i < this->getParamTypes().size(); ++i) {
            auto thisParam = this->getParamTypes()[i];
            auto otherParam = otherFunc->getParamTypes()[i];
            if (!thisParam->canBeCastedTo(otherParam))
                return false;
        }

        return true;
    }

private:
    std::vector<ZGenericParam*>* _genericDefs;
};

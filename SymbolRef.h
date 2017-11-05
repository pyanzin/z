﻿#pragma once
#include "SymbolStorage.h"
#include "ZFuncType.h"
#include "ZArrayType.h"

class SymbolEntry;

class SymbolRef {
public:
    SymbolRef(SymbolScope* storage, int id) {
        _storage = storage;
        _id = id;
    }

	SymbolEntry* getEntry() {
        return _storage->getSymbolEntries()[_id];
    }

    SymbolEntry* findSymbolDef(std::string& name, bool onlyCurrentScope = false) {
        return _storage->findSymbol(_id, name, onlyCurrentScope);
    }

	ZType* findTypeDef(std::string& name) {
		return _storage->findType(_id, name);
	}

    ZType* resolve(ZType* type) {
        ZGenericParam* gen = dynamic_cast<ZGenericParam*>(type);
        if (gen)
            return _storage->resolveGeneric(gen);

        ZArrayType* arrayType = dynamic_cast<ZArrayType*>(type);
        if (arrayType)
            return new ZArrayType(resolve(arrayType->getElementType()));

        ZFuncType* funcType = dynamic_cast<ZFuncType*>(type);
        if (funcType) {
            auto resolvedParamTypes = std::vector<ZType*>();
            for (auto paramType : funcType->getParamTypes())
                resolvedParamTypes.push_back(resolve(paramType));

            return new ZFuncType(resolve(funcType->getRetType()), resolvedParamTypes);
        }
    }

	void addResolution(ZGenericParam* param, ZType* arg) {
		_storage->addTypeArgument(param, arg);
    }
private:
    SymbolScope* _storage;
    int _id;
};

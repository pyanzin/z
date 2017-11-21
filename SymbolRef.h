#pragma once
#include "SymbolScope.h"
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
		if (dynamic_cast<ZGenericParam*>(type)) {
			while (dynamic_cast<ZGenericParam*>(type)) {
				if (_storage->isDefined(_id, type))
					return type;

				type = _storage->resolveGeneric(dynamic_cast<ZGenericParam*>(type));
			}

			return type;
        }

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

        return type;
    }

	void addResolution(ZGenericParam* param, ZType* arg) {
		_storage->addTypeArgument(param, arg);
    }

	SymbolScope* getStorage() {
		return _storage;
    }
private:
    SymbolScope* _storage;
    int _id;
};

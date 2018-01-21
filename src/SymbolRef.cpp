#include "SymbolRef.h"
#include "SymbolEntry.h"
#include "SymbolScope.h"

SymbolRef::SymbolRef(SymbolScope* storage, int id, SymbolEntry* entry) {
    _storage = storage;
    _id = id;
    _entry = entry;
}

SymbolEntry* SymbolRef::getEntry() {
    return _entry;
}

std::vector<SymbolEntry*>* SymbolRef::findSymbolDef(std::string& name, bool onlyCurrentScope) {
    return _storage->findSymbol(_id, name, onlyCurrentScope);
}

ZType* SymbolRef::findTypeDef(std::string& name) {
    return _storage->findType(_id, name);
}

ZType* SymbolRef::resolve(ZType* type) {
    if (dynamic_cast<ZGenericParam*>(type)) {
        if (_storage->isDefined(_id, type))
            return type;
        auto resolved = _storage->resolveGeneric(dynamic_cast<ZGenericParam*>(type));  
        return resolved;
    }

    if (!type->containsGenerics())
        return type;

    auto resolved = type->copyStem();

    auto typeParams = type->getTypeParams();
    for (int i = 0; i < typeParams->size(); ++i) {
        resolved->setTypeParam(i, resolve((*typeParams)[i]));
    }

    return resolved;
}

ZType* SymbolRef::findTypeOrDelayed(std::string& name) {
    ZType* type = findTypeDef(name);
    if (!type)
        return new ZDelayedType(name);
    else
        return type;
}

ZType* SymbolRef::resolveIfDelayed(ZType* type) {
    ZDelayedType* delayed = dynamic_cast<ZDelayedType*>(type);
    if (delayed)
        return findTypeDef(delayed->getName());
    else
        return type;
}

void SymbolRef::addResolution(ZGenericParam* param, ZType* arg) {
    _storage->addTypeArgument(param, arg);
}

SymbolScope* SymbolRef::getStorage() {
    return _storage;
}
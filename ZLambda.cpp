#include <string>
#include "ZFuncType.h"
#include "ZLambda.h"

void ZLambda::setType(ZType* type) {
    ZFuncType* funcType = dynamic_cast<ZFuncType*>(type);

    if (!funcType)
        throw std::exception(std::string("Unable to assign type " + type->toString() + " to lambda").c_str());

    for (int i = 0; i < _args->size(); ++i) {
        (*_args)[i]->setType(funcType->getParamTypes()[i]);
        std::string* argName = (*_args)[i]->getName();
        _ref->findSymbolDef(*argName)->setType(funcType->getParamTypes()[i]);
    }

    _body->setType(funcType->getRetType());
}

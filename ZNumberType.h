#pragma once
#include "ZBasicType.h"

class ZNumberType : public ZBasicType {
public:

    ZNumberType(llvm::Type* llvmType, std::string name, std::vector<ZType*>& canBeCastedTo)
        : ZBasicType(llvmType, name), _canBeCastedTo(canBeCastedTo) {
    }

    bool canBeCastedTo(ZType* other) override {
        if (ZType::canBeCastedTo(other))
            return true;

        for (ZType* candidate : _canBeCastedTo)
            if (other->isEqual(*candidate))
                return true;
        
        return false;
    }

private:
    std::vector<ZType*>& _canBeCastedTo;
};

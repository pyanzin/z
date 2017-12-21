#pragma once
#include <llvm/IR/Type.h>
#include "ZType.h"
#include "ZWildcardType.h"

class ZNumberType;
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

extern ZType* Void;
extern ZType* Any;
extern ZType* Unknown;

extern ZType* Int;
extern ZType* Double;
extern ZType* Char;
extern ZType* Boolean;
extern ZType* String;

extern ZWildcardType* Wildcard;
extern ZBasicType* Ref;
#pragma once
#include <llvm/IR/Type.h>
#include "ZType.h"
#include "ZWildcardType.h"

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

extern ZBasicType* Void;
extern ZBasicType* Any;
extern ZBasicType* Unknown;

extern ZBasicType* Int;
extern ZBasicType* Double;
extern ZBasicType* Char;
extern ZBasicType* Boolean;
extern ZBasicType* String;

extern ZWildcardType* Wildcard;
#include "ZType.h"

class ZDelayedType : public ZType {
public:
	ZDelayedType(std::string& delayedName) : _delayedName(delayedName) {  }
	llvm::Type* toLlvmType() override {
		throw "Error: ZDelayedType is supposed to be resolved before LLVM pass";
	}

	std::string& getName() override {
		return *new std::string("DelayedType(" + _delayedName + ")");
	}
	std::string& toString() override {
		return *new std::string("DelayedType(" + _delayedName + ")");
	}

	bool isEqual(ZType& other) override {
		return false;
	}

	std::string& getDelayedName() {
		return _delayedName;
	}
private:
	std::string _delayedName;
};

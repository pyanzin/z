#pragma once

class ZType {
public:
	virtual llvm::Type* toLlvmType() = 0;

	virtual std::string& getName() = 0;

	virtual std::string& toString() = 0;

	virtual bool isEqual(ZType& other) = 0;
};
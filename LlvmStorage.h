#pragma once
#include <map>
#include "SymbolEntry.h"

class LlvmStorage {
public:
	LlvmStorage(LlvmStorage* parent = nullptr) {
		_parent = parent;
	}

	LlvmStorage* getParent() {
		return _parent;
	}

	void add(std::string& key, llvm::Value* value) {
		_entries[key] = value;
	};

	LlvmStorage* makeChild() {
		return new LlvmStorage(this);
	}

	llvm::Value* get(std::string& key) {
		LlvmStorage* storage = this;
		do {
			auto entries = storage->_entries;			
			auto value = entries[key];
			if (value)
				return  value;
		} while (storage = storage->getParent());

		return nullptr;
	}

private:
	LlvmStorage* _parent;

	std::map<std::string, llvm::Value*> _entries;
};

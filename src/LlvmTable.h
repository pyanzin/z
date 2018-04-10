#pragma once
#include "LlvmStorage.h"

using namespace llvm;

class LlvmTable {
public:
    LlvmTable() {
        _storage = new LlvmStorage();
    }

    void enter() {
        _storage = _storage->makeChild();
    };

    void exit() {
        _storage = _storage->getParent();
    }

    void add(std::string& key, llvm::Value* value) {
        _storage->add(key, value);
    }

    llvm::Value* get(std::string& key) {
        return _storage->get(key);
    }

    llvm::Value* getAlloca(std::string& key) {
        llvm::Value* val = _storage->get(key);
        return isa<AllocaInst>(*val) ? val : nullptr;
    }

private:
    LlvmStorage* _storage;
};

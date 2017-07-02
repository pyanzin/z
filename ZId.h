#pragma once

#include "ZExpr.h"
#include "Utils.h"
#include "SymbolRef.h"

class ZId : public ZExpr {
public:
    ZId(std::string& name, SymbolRef& ref) : _name(name), _ref(ref) { }

    //llvm::Value* codegen(llvm::Function* func) override {
    //    for (auto& arg : func->args())
    //        if (arg.getName() == _name)
    //            return &(cast<Value>(arg));        
    //}

	virtual void accept(ZVisitor* visitor) {
		visitor->visit(this);
	}

	std::string& getName() {
		return _name;
    }

    SymbolRef& getRef() {
        return _ref;
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "id(" << _name.c_str() << ") : "<< getType()->toString() << "\n";
    }

private:
    std::string& _name;
    SymbolRef& _ref;
};
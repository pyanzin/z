#ifndef ZVAR
#define ZVAR

#include <llvm/IR/Value.h>
#include "ZExpr.h"
#include "Utils.h"

class ZId : public ZExpr {
public:
    ZId(std::string& name) : _name(name) { }

    //llvm::Value* codegen(llvm::Function* func) override {
    //    for (auto& arg : func->args())
    //        if (arg.getName() == _name)
    //            return &(cast<Value>(arg));        
    //}

	std::string& getName() {
		return _name;
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "id(" << _name.c_str() << ") : "<< toString(getType()) << "\n";
    }

private:
    std::string& _name;
};

#endif
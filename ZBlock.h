#ifndef ZBLOCK
#define ZBLOCK

#include "ZExpr.h"
#include <llvm/IR/Value.h>

class ZBlock : public ZExpr {
public:
    ZBlock(std::vector<ZAst*>* expressions) {
        _expressions = *expressions;
    }

    //llvm::Value* codegen(llvm::Function * func) override = 0;

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "block\n";
		for (ZAst* stmt : _expressions)
			stmt->dump(stream, depth + 1);
	}

private:
    std::vector<ZAst*> _expressions;
};

#endif
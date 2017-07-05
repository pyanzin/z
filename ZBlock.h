#ifndef ZBLOCK
#define ZBLOCK

#include "ZExpr.h"
#include <vector>

class ZBlock : public ZExpr {
public:
    ZBlock(std::vector<ZAst*>* stmts) {
		_statements = *stmts;
    }

	void accept(ZVisitor* visitor) override;

    //llvm::Value* codegen(llvm::Function * func) override = 0;

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "block\n";
		for (ZAst* stmt : _statements)
			stmt->dump(stream, depth + 1);
	}

	std::vector<ZAst*>& getStatements() {
		return _statements;
	}

    std::vector<ZAst*> _statements;
};

#endif

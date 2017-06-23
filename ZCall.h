#pragma once
#include "ZExpr.h"
#include <string>
#include <vector>
#include "Utils.h"

class ZCall : public ZExpr {
public:
    ZCall(ZAst* callee, std::vector<ZExpr*>& args) : args(args){
		this->callee = callee;
    }

    std::vector<ZExpr*>& getArgs() {
        return args;
    }

    ZAst* callee;
    std::vector<ZExpr*>& args;

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "call : " << toString(getType()) << "\n";
		callee->dump(stream, depth + 1);
		for (ZExpr* expr : args)
			expr->dump(stream, depth + 1);		
	};
};

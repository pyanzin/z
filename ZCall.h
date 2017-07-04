#pragma once
#include "ZExpr.h"
#include <string>
#include <vector>
#include "ZBasicTypes.h"

class ZCall : public ZExpr {
public:
    ZCall(ZExpr* callee, std::vector<ZExpr*>& args) : args(args){
		this->callee = callee;
    }

	void accept(ZVisitor* visitor) override;

    std::vector<ZExpr*>& getArgs() {
        return args;
    }

    ZExpr* callee;
    std::vector<ZExpr*>& args;

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "call : " << getType()->toString() << "\n";
		callee->dump(stream, depth + 1);
		for (ZExpr* expr : args)
			expr->dump(stream, depth + 1);		
	};
};
